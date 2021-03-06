#ifdef _ENABLE_MYSQLHELPER

#include "mysqlhelper.h"

#include "where.h"
#include "column.h"
#include "logger.h"
#include "helper.h"
#include "statements.h"
#include "keys.h"
#include "stringbuf.h"
#include "dblimits.h"

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

static int mysql_upsert_key_string(const UpsertStmt *const s, const char *delimiter, struct _StringBuf *sql) {
	const char *del = delimiter ? delimiter : "";
	if(!s->dbtbl->def->primarykey) {
		LOG_WARN("no primary key found");
		return 1;
	}
	if( stringbuf_appendf(sql,"%s%s%s=LAST_INSERT_ID(%s%s%s)",
			del,s->dbtbl->def->primarykey->name,del,
			del,s->dbtbl->def->primarykey->name,del ) ) {
		return 1;
	}
	for(size_t col = 0; col < s->dbtbl->def->ncols; col++) {
		if( strcmp(s->dbtbl->def->cols[col].name,s->dbtbl->def->primarykey->name) == 0) {
			continue; }
		if(!s->dbtbl->rows.isset[0][col]) {
			continue; }
		if( stringbuf_appendf(sql,",%s%s%s=VALUES(%s%s%s)",
				del,s->dbtbl->def->cols[col].name,del,
				del,s->dbtbl->def->cols[col].name,del ) ) {
			return 1;
		}
	}
	return 0;
}

int mysql_upsert_stmt_string(const UpsertStmt *const s, ValueSpecifier valspec, const char *delimiter, struct _StringBuf *sql) {
	char fmt[] = "INSERT INTO %s (%s) VALUES %s ON DUPLICATE KEY UPDATE %s";
	int rc = 0;

	StringBuf values,updatekeys,colnames;
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&updatekeys,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&colnames,SQL_VALUE_ALLOC_BLOCK);

	if(comma_concat_colnames_setonly(&colnames,s->dbtbl,delimiter)) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT;	}

	if( upsert_values_row_string(s->dbtbl, valspec, &values, 0) ) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT; }

	if( mysql_upsert_key_string(s, delimiter, &updatekeys) ) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT; }

	if( stringbuf_appendf(sql,fmt,
			s->dbtbl->def->name,stringbuf_get(&colnames),
			(stringbuf_get(&values) ? stringbuf_get(&values) : ""),
			(stringbuf_get(&updatekeys) ? stringbuf_get(&updatekeys) : "")) ) {
		rc = 1;
		goto MYSQL_UPSERT_STMT_STRING_EXIT; }
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

MYSQL_UPSERT_STMT_STRING_EXIT:
	stringbuf_destroy(&colnames);
	stringbuf_destroy(&updatekeys);
	stringbuf_destroy(&values);
	return rc;
}

#endif
