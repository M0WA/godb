#ifndef _DISABLE_MYSQL

#include "mysqlhelper.h"

#include "where.h"
#include "column.h"
#include "columntypes.h"
#include "logger.h"
#include "helper.h"
#include "selectresult.h"

#include <time.h>
#include <stdlib.h>
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
		if(col->size != 0 && col->size <= 16) {
			*ft = MYSQL_TYPE_SHORT;
		} else if(col->size <= 32 || col->size == 0) {
			*ft = MYSQL_TYPE_LONG;
		} else if (col->size <= 64) {
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

static int mysql_time(const struct tm *const t,MYSQL_TIME *mt) {
	mt->year=t->tm_year + 1900;
	mt->month=t->tm_mon;
	mt->day=t->tm_mday;
	mt->hour=t->tm_hour;
	mt->minute=t->tm_min;
	mt->second=t->tm_sec;
	return 0;
}

static int mysql_where_comp(const struct _WhereComposite *comp, MySQLBindWrapper *wrapper) {
	for(size_t i = 0; i < comp->cnt; i++) {
		if( mysql_where(comp->where[i],wrapper) ) {
			return 1; }
	}
	return 1;
}

static int mysql_where_cond(const struct _WhereCondition *cond, MySQLBindWrapper *wrapper) {
	if(cond->cnt > 1 && cond->cond != WHERE_EQUAL && cond->cond != WHERE_NOT_EQUAL) {
		LOG_WARN("only equal/not equal allow for range types in where clause");
		return 1; }

	unsigned long buffer_length = mysql_get_colbuf_size(cond->def);
	for(size_t i = 0,pos = 0; i < cond->cnt; i++, pos += buffer_length) {
		if( mysql_bind_append(cond->def,cond->values[i],wrapper) ) {
			return 1; }
	}
	return 0;
}

int mysql_where(const struct _WhereClause *clause,struct _MySQLBindWrapper *wrapper) {
	if(clause->cnt == 0) {
		return 0; }
	for(size_t i = 0; i < clause->cnt; i++) {
		switch(clause->stmts[i]->cond.type) {
		case WHERE_COND:
			if( mysql_where_cond(&clause->stmts[i]->cond, wrapper) ) {
				LOG_WARN("invalid condition for mysql");
				return 1;
			}
			break;
		case WHERE_COMP:
			for(size_t j = 0; j < clause->stmts[i]->comp.cnt; j++) {
				if( mysql_where_comp(&clause->stmts[i]->comp, wrapper) ) {
					LOG_WARN("invalid composite for mysql");
					return 1;
				}
			}
			break;
		}
	}
	return 0;
}

int mysql_bind_append(const struct _DBColumnDef *def,const void *val,MySQLBindWrapper *wrapper) {
	if(wrapper->bind_idx >= MAX_MYSQL_BIND_COLS) {
		LOGF_WARN("mysql allows only %d values in statements, change MAX_MYSQL_BIND_COLS to a higher value if you need more",MAX_MYSQL_BIND_COLS);
		return 1;
	}
	memset(&wrapper->bind[wrapper->bind_idx],0,sizeof(MYSQL_BIND));
	wrapper->is_null[wrapper->bind_idx] = 0;

	if( mysql_datatype(def,&wrapper->bind[wrapper->bind_idx].buffer_type) ) {
		LOG_WARN("mysql_datatype: error");
		return 1; }
	wrapper->bind[wrapper->bind_idx].buffer_length = mysql_get_colbuf_size(def);

	if(!val) {
		wrapper->is_null[wrapper->bind_idx] = 1;
		wrapper->bind[wrapper->bind_idx].is_null = &wrapper->is_null[wrapper->bind_idx];
		wrapper->bind[wrapper->bind_idx].buffer = 0;
		wrapper->bind[wrapper->bind_idx].length = 0;
	} else if(def->type == COL_TYPE_DATETIME) {
		memset(&wrapper->times[wrapper->times_idx],0,sizeof(MYSQL_TIME));
		if( mysql_time((const struct tm *const)(val),&wrapper->times[wrapper->times_idx]) ) {
			LOG_WARN("mysql_time: error");
			return 1; }
		wrapper->bind[wrapper->bind_idx].buffer = &(wrapper->times[wrapper->times_idx]);
		wrapper->bind[wrapper->bind_idx].length = 0;
		wrapper->times_idx++;
	} else {
		wrapper->bind[wrapper->bind_idx].is_null = 0;
		wrapper->bind[wrapper->bind_idx].buffer = (void*)val;
		wrapper->bind[wrapper->bind_idx].length = 0;

		if(def->type == COL_TYPE_STRING) {
			wrapper->str_length[wrapper->str_idx] = strlen((char *)val);
			wrapper->bind[wrapper->bind_idx].length = &wrapper->str_length[wrapper->str_idx];
			wrapper->str_idx++;
		}
	}
	wrapper->bind_idx++;
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

#endif
