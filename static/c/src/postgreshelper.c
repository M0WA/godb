#ifdef _ENABLE_POSTGRESHELPER

#include "postgreshelper.h"

#include "where.h"
#include "column.h"
#include "helper.h"
#include "logger.h"
#include "statements.h"
#include "keys.h"
#include "stringbuf.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alloca.h>


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
			if(printed && append_string(" OR ",sql)) {
				return 1; }
			if(*prefix_left) {
				if( append_string(prefix_left,sql) || append_string(".",sql) ) {
					return 1; }
			}
			if(append_string(s->uniqs[ukey]->cols[col],sql) || append_string(" = ",sql)) {
				return 1; }
			if(*prefix_right) {
				if( append_string(prefix_right,sql) || append_string(".",sql) ) {
					return 1; }
			}
			if(append_string(s->uniqs[ukey]->cols[col],sql)){
				return 1; }
			printed++;
		}
	}
	return 1;
}

static int postgres_upsert_set_string(const UpsertStmt *const s, const char *prefix, ValueSpecifier valspec, struct _StringBuf *sql) {
	for(size_t col = 0; col < s->ncols; col++) {
		if(col && append_string(",",sql)) {
			return 1;
		}
		if(	append_string(s->defs[col].name,sql) ||
			append_string("=",sql) ||
			append_string(prefix,sql) ||
			append_string(".",sql) ||
			append_string(s->defs[col].name,sql)
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
  (VALUES (%s)), \
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
	char *values = 0;
	char *updateset = 0;
	char *where_update = 0;
	char *where_insert = 0;
	int rc = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols, 0);
	if(!colnames) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	if( insert_values_row_string(s->defs, s->ncols, valspec, s->valbuf, s->nrows, &values, 0, 0) ) {
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

	size_t stmtlen = 1 + strlen(fmt) + strlen(prefix_values) + strlen(colnames) + strlen(values) + strlen(alias_upsert) +
			strlen(s->defs[0].table) + strlen(prefix_update_result) + strlen(updateset) +
			strlen(prefix_values) + strlen(prefix_update_subselect) + strlen(where_update) + strlen(prefix_update_result) +
			//insert
			strlen(s->defs[0].table) + strlen(colnames) + strlen(colnames) + strlen(prefix_values) + strlen(alias_upsert) + strlen(where_insert);

	*sql = malloc(stmtlen);
	snprintf(*sql,stmtlen,fmt,
			prefix_values,colnames,values,alias_upsert,
			s->defs[0].table,prefix_update_result,updateset,
			prefix_values,prefix_update_subselect,where_update,prefix_update_result,
			s->defs[0].table,colnames,colnames,prefix_values,alias_upsert,where_insert);

POSTGRES_UPSERT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	if(values) {
		free(values); }
	if(updateset) {
		free(updateset); }
	if(where_update) {
		free(where_update);	}
	if(where_insert) {
		free(where_insert);	}
	return rc;
}

#endif
