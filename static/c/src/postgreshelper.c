#ifdef _ENABLE_POSTGRESHELPER

#include "postgreshelper.h"

#include "where.h"
#include "column.h"
#include "helper.h"
#include "logger.h"
#include "statements.h"
#include "keys.h"
#include "stringbuf.h"
#include "dblimits.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alloca.h>

int postgres_raw_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial) {
	if(def->autoincrement) {
		return stringbuf_append(sql,"DEFAULT");
	} else {
		return values_generic_value_specifier(def,value,sql,serial);
	}
	return 1;
}

int postgres_time_to_tm(const char *val, struct tm *t) {
	memset(t, 0, sizeof(struct tm));
	if( !strptime(val, "%Y-%m-%d %H:%M:%S", t) ) {
		LOGF_WARN("invalid postgres date: %s",val);
		return 1;
	}
	return 0;
}

static int postgres_upsert_where_string(const UpsertStmt *const s, const char *prefix_left, const char *prefix_right, struct _StringBuf *sql) {
	size_t printed = 0;
	for(size_t ukey = 0; ukey < s->nuniq; ukey++) {
		for(size_t col = 0; col < s->uniqs[ukey]->ncols; col++) {
			if(printed && stringbuf_append(sql," OR ")) {
				return 1; }
			if(*prefix_left) {
				if( stringbuf_append(sql,prefix_left) || stringbuf_append(sql,".") ) {
					return 1; }
			}
			if(stringbuf_append(sql,s->uniqs[ukey]->cols[col]) || stringbuf_append(sql," = ")) {
				return 1; }
			if(*prefix_right) {
				if( stringbuf_append(sql,prefix_right) || stringbuf_append(sql,".") ) {
					return 1; }
			}
			if(stringbuf_append(sql,s->uniqs[ukey]->cols[col])){
				return 1; }
			printed++;
		}
	}
	return 0;
}

static int postgres_upsert_set_string(const UpsertStmt *const s, const char *prefix, ValueSpecifier valspec, struct _StringBuf *sql) {
	for(size_t col = 0; col < s->ncols; col++) {
		if(col && stringbuf_append(sql,",")) {
			return 1;
		}
		if(	stringbuf_append(sql,s->defs[col].name) ||
			stringbuf_append(sql,"=") ||
			stringbuf_append(sql,prefix) ||
			stringbuf_append(sql,".") ||
			stringbuf_append(sql,s->defs[col].name)
		) {
			return 1;
		}
	}
	return 0;
}

//example:
/*
WITH new_values
  (domain)
AS
  (VALUES ('domainname') ),
upsert AS
(
  UPDATE deepnet.public.secondleveldomains m
  SET
	domain = nv.domain
  FROM new_values nv
  WHERE
	m.domain = nv.domain
  RETURNING m.*
)
INSERT INTO deepnet.public.secondleveldomains (domain)
SELECT
  domain
FROM new_values
WHERE NOT EXISTS
(
  SELECT
	1
  FROM
	upsert up
  WHERE
	domain = new_values.domain
)
*/
int postgres_upsert_stmt_string(const UpsertStmt *const s, ValueSpecifier valspec, struct _StringBuf *sql) {
	const char fmt[] = " \
WITH %s \
  (%s) \
AS \
  (VALUES %s), \
%s AS \
( \
  UPDATE %s %s \
  SET %s \
  FROM %s %s \
  WHERE \
	%s \
  RETURNING %s.* \
) \
INSERT INTO %s (%s) \
SELECT \
  %s \
FROM %s \
WHERE NOT EXISTS \
( \
  SELECT \
	1 \
  FROM \
	%s up \
  WHERE \
	%s \
)";
	const char *alias_upsert = "upsert";
	const char *prefix_update_subselect = "nv";
	const char *prefix_update_result = "updated";
	const char *prefix_values = "new_values";
	char *colnames = 0;
	int rc = 0;

	StringBuf updateset,values,where_update,where_insert;
	stringbuf_init(&updateset,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&where_update,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&where_insert,SQL_VALUE_ALLOC_BLOCK);

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	if( insert_values_row_string(s->defs, s->ncols, valspec, s->valbuf, s->nrows, &values, 0) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	if( postgres_upsert_set_string(s, prefix_update_subselect, valspec, &updateset) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	if( postgres_upsert_where_string(s, prefix_update_result, prefix_update_subselect, &where_update) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	if( postgres_upsert_where_string(s, "", prefix_values, &where_insert) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	size_t sqlsize = 1 + strlen(fmt) + strlen(prefix_values) + strlen(colnames) + stringbuf_strlen(&values) + strlen(alias_upsert) +
			strlen(s->defs[0].table) + strlen(prefix_update_result) + stringbuf_strlen(&updateset) +
			strlen(prefix_values) + strlen(prefix_update_subselect) + stringbuf_strlen(&where_update) + strlen(prefix_update_result) +
			//insert
			strlen(s->defs[0].table) + strlen(colnames) + strlen(colnames) + strlen(prefix_values) + strlen(alias_upsert) + stringbuf_strlen(&where_insert);

	if( stringbuf_resize(sql,sqlsize) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	snprintf(stringbuf_buf(sql),sqlsize,fmt,
			prefix_values,colnames,stringbuf_get(&values),alias_upsert,
			s->defs[0].table,prefix_update_result,stringbuf_get(&updateset),
			prefix_values,prefix_update_subselect,stringbuf_get(&where_update),prefix_update_result,
			s->defs[0].table,colnames,colnames,prefix_values,alias_upsert,stringbuf_get(&where_insert));
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

POSTGRES_UPSERT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	stringbuf_destroy(&updateset);
	stringbuf_destroy(&values);
	stringbuf_destroy(&where_update);
	stringbuf_destroy(&where_insert);
	return rc;
}

#endif
