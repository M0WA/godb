#ifndef _DISABLE_MYSQL

#include "mysqlbind.h"

#include "column.h"
#include "where.h"
#include "logger.h"
#include "mysqlhelper.h"

#include <string.h>
#include <time.h>

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

#endif
