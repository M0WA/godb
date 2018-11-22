#pragma once

#ifndef _DISABLE_MYSQL

#include "dblimits.h"
#include <mysql/mysql.h>

struct _DBColumnDef;
struct _WhereClause;
struct _SelectResult;
struct tm;

typedef struct _MySQLBindWrapper {
	MYSQL_BIND bind[MAX_MYSQL_BIND_COLS];
	my_bool is_null[MAX_MYSQL_BIND_COLS];
	size_t bind_idx;
	unsigned long str_length[MAX_MYSQL_BIND_COLS];
	size_t str_idx;
	MYSQL_TIME times[MAX_MYSQL_BIND_COLS];
	size_t times_idx;
} MySQLBindWrapper;

int mysql_tm(const MYSQL_TIME *mt, struct tm *const t);
int mysql_bind_append(const struct _DBColumnDef *def,const void *val,MySQLBindWrapper *wrapper);
int mysql_where(const struct _WhereClause *clause,struct _MySQLBindWrapper *wrapper,char** sql);
size_t mysql_get_colbuf_size(const struct _DBColumnDef *const col);
int mysql_datatype(const struct _DBColumnDef *const col,enum enum_field_types *ft);
//int mysql_selectresult_from_stmt(const struct _DBColumnDef *cols,size_t ncols,struct _SelectResult* res,MYSQL_STMT *stmt);

#endif
