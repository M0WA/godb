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

int postgres_raw_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial) {
	if(def->autoincrement) {
		return stringbuf_append(sql,"DEFAULT");
	} else {
		int rc = values_generic_value_specifier(def,value,sql,serial);
		if(!rc && def->type == COL_TYPE_DATETIME) {
			return stringbuf_append(sql,"::timestamp");
		}
		return rc;
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

static int postgres_upsert_leftjoin_string(const UpsertStmt *const s, const char *prefix_values, const char *alias_upsert, const char *delimiter, struct _StringBuf *sql) {
	const char *del = delimiter ? delimiter : "";
	if(stringbuf_append(sql," (")) {
		return 1; }
	for(size_t ukey = 0; ukey < s->dbtbl->def->nunique; ukey++) {
		if(ukey && stringbuf_append(sql," OR")) {
			return 1; }
		if(stringbuf_append(sql," (")) {
			return 1; }
		for(size_t col = 0; col < s->dbtbl->def->uniquekeys[ukey].ncols; col++) {
			if(col && stringbuf_append(sql," AND ")) {
				return 1; }
			if( stringbuf_appendf(sql,"%s%s%s.%s%s%s = %s%s%s.%s%s%s",
					del,prefix_values,del,
					del,s->dbtbl->def->uniquekeys[ukey].cols[col]->name,del,
					del,alias_upsert,del,
					del,s->dbtbl->def->uniquekeys[ukey].cols[col]->name,del) ) {
				return 1; }
		}
		if(stringbuf_append(sql,")")) {
			return 1; }
	}
	if(stringbuf_append(sql,")")) {
		return 1; }
	return 0;
}

static int postgres_upsert_where_string(const UpsertStmt *const s, const char *prefix_left, const char *prefix_right, const char *delimiter, struct _StringBuf *sql) {
	size_t p = 0;
	const char *del = delimiter ? delimiter : "";
	for(size_t ukey = 0; ukey < s->dbtbl->def->nunique; ukey++) {
		for(size_t col = 0; col < s->dbtbl->def->uniquekeys[ukey].ncols; col++) {
			/*
			//TODO: check if column is actually set
			if(!s->dbtbl->rows.isset[0][col]) {
				continue; }
			*/
			if(p && stringbuf_append(sql," AND ")) {
				return 1; }
			if(*prefix_left && stringbuf_appendf(sql,"%s%s%s.",del,prefix_left,del) ) {
				return 1; }
			if(stringbuf_appendf(sql,"%s%s%s = ",del,s->dbtbl->def->uniquekeys[ukey].cols[col]->name,del) ) {
				return 1; }
			if(*prefix_right && stringbuf_appendf(sql,"%s%s%s.",del,prefix_right,del) ) {
				return 1; }
			if(stringbuf_appendf(sql,"%s%s%s",del,s->dbtbl->def->uniquekeys[ukey].cols[col]->name,del)){
				return 1; }
			p++;
		}
	}
	return 0;
}

static int postgres_upsert_set_string(const UpsertStmt *const s, const char *prefix, ValueSpecifier valspec, const char *delimiter, struct _StringBuf *sql) {
	size_t p = 0;
	const char *del = delimiter ? delimiter : "";
	for(size_t col = 0; col < s->dbtbl->def->ncols; col++) {
		if(!s->dbtbl->rows.isset[0][col]) {
			continue; }
		if(p && stringbuf_append(sql,",")) {
			return 1; }
		if(	stringbuf_appendf(sql,"%s%s%s = %s%s%s.%s%s%s",
				del,s->dbtbl->def->cols[col].name,del,
				del,prefix,del,del,s->dbtbl->def->cols[col].name,del) ) {
			return 1; }
		p++;
	}
	return 0;
}

static int postgres_upsert_select_column_string(const UpsertStmt *const s, const char *prefix_update_result,const char *prefix_update_subselect, const char *delimiter, struct _StringBuf *sql) {
	const char *del = delimiter ? delimiter : "";
	for(size_t col = 0; col < s->dbtbl->def->ncols; col++) {
		if(	col && stringbuf_append(sql,", ") ) {
			return 1;
		}
		if(!s->dbtbl->rows.isset[0][col]) {
			if(	stringbuf_appendf(sql,"%s%s%s.%s%s%s",del,prefix_update_result,del,del,s->dbtbl->def->cols[col].name,del) ) {
				return 1;
			}
		} else {
			if(	stringbuf_appendf(sql,"%s%s%s.%s%s%s",del,prefix_update_subselect,del,del,s->dbtbl->def->cols[col].name,del) ) {
				return 1;
			}
		}
	}
	return 0;
}

int postgres_upsert_stmt_string(const UpsertStmt *const s, ValueSpecifier valspec, const char *delimiter, struct _StringBuf *sql) {
	if(s->dbtbl->def->nunique > 1 || s->dbtbl->def->uniquekeys[0].ncols > 1) {
		LOG_ERROR("postgres does not support combined unique key updates as well as tables with more than one unique key");
		return 1;
	}

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
LEFT JOIN %s ON %s \
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
	int rc = 0;

	StringBuf updateset,values,where_update,where_insert, colnames_select, colnames, colnames_setonly,leftjoin;
	stringbuf_init(&updateset,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&where_update,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&where_insert,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&colnames_select,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&colnames,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&colnames_setonly,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&leftjoin,SQL_VALUE_ALLOC_BLOCK);

	if(comma_concat_colnames_setonly(&colnames_setonly,s->dbtbl,delimiter)) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( comma_concat_colnames_insert(&colnames,s->dbtbl->def->cols,s->dbtbl->def->ncols,delimiter) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( postgres_upsert_select_column_string(s, alias_upsert, prefix_values, delimiter, &colnames_select) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( upsert_values_row_string(s->dbtbl, postgres_raw_value_specifier, &values, 0) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( postgres_upsert_set_string(s, prefix_update_subselect, valspec, delimiter, &updateset) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( postgres_upsert_where_string(s, prefix_update_result, prefix_update_subselect, delimiter, &where_update) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( postgres_upsert_where_string(s, "", prefix_values, delimiter, &where_insert) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }
	if( postgres_upsert_leftjoin_string(s, prefix_values,alias_upsert, delimiter, &leftjoin) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	if( stringbuf_appendf(sql,fmt,
			prefix_values,
			stringbuf_get(&colnames_setonly),
			stringbuf_get(&values),
			alias_upsert,
			s->dbtbl->def->name, prefix_update_result,
			stringbuf_get(&updateset),
			prefix_values,prefix_update_subselect,
			stringbuf_get(&where_update),prefix_update_result,
			s->dbtbl->def->name,stringbuf_buf(&colnames),stringbuf_buf(&colnames_select),
			alias_upsert,prefix_values,stringbuf_get(&leftjoin),alias_upsert,stringbuf_get(&where_insert)) ) {
		rc = 1;
		goto POSTGRES_UPSERT_STMT_STRING_EXIT; }

	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

POSTGRES_UPSERT_STMT_STRING_EXIT:
	stringbuf_destroy(&leftjoin);
	stringbuf_destroy(&colnames_setonly);
	stringbuf_destroy(&colnames);
	stringbuf_destroy(&colnames_select);
	stringbuf_destroy(&updateset);
	stringbuf_destroy(&values);
	stringbuf_destroy(&where_update);
	stringbuf_destroy(&where_insert);
	return rc;
}

#endif
