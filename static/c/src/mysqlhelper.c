#ifdef _ENABLE_MYSQLHELPER

#include "mysqlhelper.h"

#include "where.h"
#include "column.h"
#include "logger.h"
#include "helper.h"
#include "statements.h"

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

int mysql_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial) {
	if( append_string("?",sql) ) {
		return 1; }
	return 0;
}

int mysql_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial) {
	if( append_string("?",sql) ) {
		return 1; }
	return 0;
}

int mysql_upsert_stmt_string(const UpsertStmt *const s, char** sql) {
	//const char fmt[] = "INSERT INTO `%s`.`%s` (%s) VALUES(%s) ON DUPLICATE KEY UPDATE %s";
	return 1;
}

#endif
