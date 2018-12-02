#ifdef _ENABLE_MYSQLHELPER

#include "mysqlhelper.h"

#include "where.h"
#include "column.h"
#include "logger.h"
#include "helper.h"
#include "statements.h"
#include "keys.h"

#include <time.h>
#include <string.h>

size_t mysql_get_colbuf_size(const struct _DBColumnDef *const col) {
	if(col->type == COL_TYPE_DATETIME) {
		return sizeof(MYSQL_TIME);
	}
	return get_column_bufsize(col);
}

int mysql_datatype(const struct _DBColumnDef *const col,enum enum_field_types *ft) {
	switch(col->type) {
	case COL_TYPE_STRING:
		*ft = MYSQL_TYPE_STRING;
		break;
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= sizeof(short)) {
			*ft = MYSQL_TYPE_SHORT;
		} else if(col->size <= sizeof(long) || col->size == 0) {
			*ft = MYSQL_TYPE_LONG;
		} else if (col->size <= sizeof(long long)) {
			*ft = MYSQL_TYPE_LONGLONG;
		} else {
			LOGF_WARN("invalid int size for mysql: %lu",col->size);
			return 1;
		}
		break;
	case COL_TYPE_FLOAT:
		*ft = MYSQL_TYPE_DOUBLE;
		break;
	case COL_TYPE_DATETIME:
		*ft = MYSQL_TYPE_TIMESTAMP;
		break;
	default:
		LOG_WARN("invalid datatype for mysql");
		return 1;
	}
	return 0;
}

int mysql_tm(const MYSQL_TIME *mt, struct tm *const t) {
	t->tm_year = mt->year -1900;
	t->tm_mon = mt->month;
	t->tm_mday = mt->day;
	t->tm_hour = mt->hour;
	t->tm_min = mt->minute;
	t->tm_sec = mt->second;
	return 0;
}

int mysql_string_to_tm(const char *val, struct tm *t) {
	memset(t, 0, sizeof(struct tm));
	if( !strptime(val, "%Y-%m-%d %H:%M:%S", t) ) {
		LOGF_WARN("invalid postgres date: %s",val);
		return 1;
	}
	return 0;
}

static int mysql_upsert_key_string(const UpsertStmt *const s, struct _StringBuf *sql) {
	if(!s->prikey) {
		LOG_WARN("no primary key found");
		return 1;
	}
	if( append_string(s->prikey->name,sql) ||
	    append_string(" = LAST_INSERT_ID(",sql) ||
		append_string(s->prikey->name,sql) ||
		append_string(")",sql)
	){
		return 1;
	}
	for(size_t col = 0; col < s->ncols; col++) {
		if( strcmp(s->defs[col].name,s->prikey->name) == 0) {
			continue; }
		if(append_string(",",sql)) {
			return 1; }
		if ( append_string(s->defs[col].name,sql) ||
			append_string(" = VALUES(",sql) ||
			append_string(s->defs[col].name,sql) ||
			append_string(")",sql)
		){
			return 1;
		}
	}
	return 0;
}

int mysql_upsert_stmt_string(const UpsertStmt *const s, ValueSpecifier valspec, struct _StringBuf *sql) {
	char fmt[] = "INSERT INTO %s (%s) VALUES %s ON DUPLICATE KEY UPDATE %s";
	char *colnames = 0;
	char *values = 0;
	char *updatekeys = 0;
	int rc = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols, 0);
	if(!colnames) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT;	}

	if( insert_values_row_string(s->defs, s->ncols, valspec, s->valbuf, s->nrows, &values, 0, 0) ) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT; }

	if( mysql_upsert_key_string(s, &updatekeys) ) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT; }

	size_t lenStmt = 1 + (values ? strlen(values) : 0) + strlen(fmt) + strlen(colnames) + strlen(s->defs->table) + (updatekeys ? strlen(updatekeys) : 0);
	*sql = malloc(lenStmt);
	if(!*sql) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT; }
	sprintf(*sql,fmt,s->defs->table,colnames,(values ? values : ""),(updatekeys ? updatekeys : ""));
	LOGF_DEBUG("statement: %s",*sql);

MYSQL_UPSERT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	if(values) {
		free(values); }
	if(updatekeys) {
		free(updatekeys); }
	return rc;
}

#endif
