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

static int mysql_datatype(const struct _DBColumnDef *const col,enum enum_field_types *ftype,unsigned long *buffer_length) {
	enum enum_field_types ftmp;
	enum enum_field_types *ft = ftype ? ftype : &ftmp;
	*buffer_length = 0;
	switch(col->type) {
	case COL_TYPE_STRING:
		*ft = MYSQL_TYPE_STRING;
		*buffer_length = (col->size + 1);
		break;
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= 16) {
			*ft = MYSQL_TYPE_SHORT;
			*buffer_length = sizeof(short);
		} else if(col->size <= 32 || col->size == 0) {
			*ft = MYSQL_TYPE_LONG;
			*buffer_length = sizeof(long);
		} else if (col->size <= 64) {
			*ft = MYSQL_TYPE_LONGLONG;
			*buffer_length = sizeof(long long);
		} else {
			LOGF_WARN("invalid int size for mysql: %lu",col->size);
			return 1;
		}
		break;
	case COL_TYPE_FLOAT:
		*ft = MYSQL_TYPE_DOUBLE;
		*buffer_length = sizeof(double);
		break;
	case COL_TYPE_DATETIME:
		*ft = MYSQL_TYPE_TIMESTAMP;
		*buffer_length = sizeof(MYSQL_TIME);
		break;
	default:
		LOG_WARN("invalid datatype for mysql");
		return 1;
	}
	return 0;
}

static int mysql_time(const struct tm *const t,MYSQL_TIME* mt) {
	mt->year=t->tm_year + 1900;
	mt->month=t->tm_mon;
	mt->day=t->tm_mday;
	mt->hour=t->tm_hour;
	mt->minute=t->tm_min;
	mt->second=t->tm_sec;
	return 0;
}

static int mysql_comp_op(WhereCompOperator op,char** sql) {
	if(sql || (*sql) == 0) {
		return 1; }
	switch(op) {
	case WHERE_AND:
		return append_string(") AND (", sql);
	case WHERE_OR:
		return append_string(") OR (", sql);
	default:
		return 1;
	}
}

static int mysql_where_comp(const struct _WhereComposite* comp, MySQLBindWrapper* wrapper,char** sql) {
	for(size_t i = 0; i < comp->cnt; i++) {
		if(wrapper->bind_idx) {
			if( mysql_comp_op(comp->where[i]->comp,sql) ) {
				return 1; }
		}
		if( mysql_where(comp->where[i],wrapper,sql) ) {
			return 1; }
	}
	return 1;
}

static int mysql_where_cond(const struct _WhereCondition* cond, MySQLBindWrapper* wrapper,char** sql) {
	if(cond->cnt > 1 && cond->cond != WHERE_EQUAL && cond->cond != WHERE_NOT_EQUAL) {
		LOG_WARN("only equal/not equal allow for range types in where clause");
		return 1; }

	if(append_string("(", sql)) {
		return 1;}
	if(append_string(cond->def->name, sql)) {
		return 1;}
	if(cond->cnt > 1) {
		switch(cond->cond) {
		case WHERE_EQUAL:
			if(append_string(" IN (", sql)) {
				return 1;}
			break;
		case WHERE_NOT_EQUAL:
			if(append_string(" NOT IN (", sql)) {
				return 1;}
			break;
		default:
			LOG_WARN("multi column in where clause cannot combined with 'like' or is null operator");
			return 1;
		}
	} else {
		switch(cond->cond) {
		case WHERE_EQUAL:
			if(append_string(" = ?", sql)) {
				return 1;}
			break;
		case WHERE_NOT_EQUAL:
			if(append_string(" != ?", sql)) {
				return 1;}
			break;
		case WHERE_LIKE:
			if(append_string(" LIKE %?%", sql)) {
				return 1;}
			break;
		case WHERE_NOT_LIKE:
			if(append_string(" NOT LIKE %?%", sql)) {
				return 1;}
			break;
		case WHERE_IS_NULL:
			if(append_string(" IS NULL ", sql)) {
				return 1;}
			break;
		case WHERE_IS_NOT_NULL:
			if(append_string(" IS NOT NULL ", sql)) {
				return 1;}
			break;
		default:
			LOG_WARN("column in where clause cannot combined with 'like' operator");
			return 1;
		}
	}

	unsigned long buffer_length;
	if( mysql_datatype(cond->def,0,&buffer_length) ) {
		return 1; }
	for(size_t i = 0,pos = 0; i < cond->cnt; i++, pos += buffer_length) {
		if( mysql_bind_append(cond->def,(cond->values+pos),wrapper) ) {
			return 1; }
		if(cond->cnt > 1) {
			if(i && append_string(", ", sql)) {
				return 1; }
			if(append_string("?", sql)) {
				return 1; }
		}
	}
	if(cond->cnt > 1 && append_string(")", sql)) {
		return 1; }
	if(append_string(")", sql)) {
		return 1;}
	return 0;
}

int mysql_where(const struct _WhereClause *clause,struct _MySQLBindWrapper *wrapper,char** sql) {
	if(clause->cnt == 0) {
		return 0; }
	if(append_string("(", sql)) {
		return 1;}
	for(size_t i = 0; i < clause->cnt; i++) {
		switch(clause->stmts[i]->cond.type) {
		case WHERE_COND:
			if( mysql_where_cond(&clause->stmts[i]->cond, wrapper,sql) ) {
				LOG_WARN("invalid condition for mysql");
				return 1;
			}
			break;
		case WHERE_COMP:
			for(size_t j = 0; j < clause->stmts[i]->comp.cnt; j++) {
				if( mysql_where_comp(&clause->stmts[i]->comp, wrapper,sql) ) {
					LOG_WARN("invalid composite for mysql");
					return 1;
				}
			}
			break;
		}
	}
	if(append_string(")", sql)) {
			return 1;}
	return 0;
}

int mysql_bind_append(const struct _DBColumnDef *def,const void *val,MySQLBindWrapper *wrapper) {
	if(wrapper->bind_idx >= MAX_MYSQL_BIND_COLS) {
		LOGF_WARN("mysql allows only %d values in statements, change MAX_MYSQL_BIND_COLS to a higher value if you need more",MAX_MYSQL_BIND_COLS);
		return 1;
	}
	memset(&wrapper->bind[wrapper->bind_idx],0,sizeof(MYSQL_BIND));
	wrapper->is_null[wrapper->bind_idx] = 0;

	if( mysql_datatype(def,&wrapper->bind[wrapper->bind_idx].buffer_type,&wrapper->bind[wrapper->bind_idx].buffer_length) ) {
		LOG_WARN("mysql_datatype: error");
		return 1; }

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
			wrapper->str_length[wrapper->str_idx] = strlen((char *)val) + 1;
			wrapper->bind[wrapper->bind_idx].length = &wrapper->str_length[wrapper->str_idx];
			wrapper->str_idx++;
		}
	}
	wrapper->bind_idx++;
	return 0;
}

int mysql_selectresult_from_stmt(const struct _DBColumnDef *cols,size_t ncols,struct _SelectResult** res,MYSQL_STMT *stmt) {
	MYSQL_BIND bind[MAX_MYSQL_BIND_COLS];
	my_bool is_null[MAX_MYSQL_BIND_COLS];
	unsigned long length[MAX_MYSQL_BIND_COLS];
	memset(&bind,0,sizeof(MYSQL_BIND) * MAX_MYSQL_BIND_COLS);
	memset(&is_null,0,sizeof(my_bool) * MAX_MYSQL_BIND_COLS);
	memset(&length,0,sizeof(unsigned long) * MAX_MYSQL_BIND_COLS);

	size_t row_size = 0;
	for(size_t i = 0; i < ncols; i++) {
		if( mysql_datatype(&cols[i],&bind[i].buffer_type,&bind[i].buffer_length) ) {
			return 1; }
		row_size += bind[i].buffer_length;
		bind[i].length = &length[i];
		bind[i].is_null = &is_null[i];
	}

	*res = malloc(sizeof(struct _SelectResult));
	if(!*res) {
		return 1; }
	memset(*res,0,sizeof(struct _SelectResult));
	(*res)->cols = cols;
	(*res)->ncols = ncols;

	(*res)->nrows = 0;
	while(1) {
		if(!(*res)->rows) {
			(*res)->rows = malloc(row_size);
			if(!*res) {
				return 1; }
		} else {
			void** tmp = realloc((*res)->rows,row_size * ((*res)->nrows + 1));
			if(!tmp) {
				return 1; }
			(*res)->rows = tmp;
		}
		void* rowbuf = (*res)->rows + (row_size * (*res)->nrows);
		memset(rowbuf,0,row_size);

		for(size_t j = 0,b = 0; j < ncols; j++) {
			void* colbuf = (rowbuf + b);
			size_t rowidx = (ncols * (*res)->nrows) + j;
			bind[rowidx].buffer = colbuf;
			bind[rowidx].length = &length[rowidx];
			bind[rowidx].is_null = &is_null[rowidx];
			bind[rowidx].buffer_length = length[rowidx];
			*(bind[rowidx].length) = length[rowidx];
			b += length[rowidx];
		}
		if (mysql_stmt_bind_result(stmt, &(bind[0]))) {
			LOGF_WARN("mysql_stmt_bind_result: %s", mysql_stmt_error(stmt));
			return 1;
		}
		if ( mysql_stmt_fetch(stmt) ) {
			break;
		}
		(*res)->nrows++;
	}
	return 0;
}

#endif
