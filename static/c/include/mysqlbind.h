#pragma once

#ifndef _DISABLE_MYSQL

struct _WhereClause;
struct _DBColumnDef;

#include "dblimits.h"

#include <stddef.h>
#include <mysql/mysql.h>

typedef struct _MySQLBindWrapper {
	MYSQL_BIND bind[MAX_MYSQL_BIND_COLS];
	my_bool is_null[MAX_MYSQL_BIND_COLS];
	size_t bind_idx;
	unsigned long str_length[MAX_MYSQL_BIND_COLS];
	size_t str_idx;
	MYSQL_TIME times[MAX_MYSQL_BIND_COLS];
	size_t times_idx;
} MySQLBindWrapper;

int mysql_bind_append(const struct _DBColumnDef *def,const void *val,MySQLBindWrapper *wrapper);
int mysql_where(const struct _WhereClause *clause,struct _MySQLBindWrapper *wrapper);
int mysql_values(const struct _DBColumnDef *def,size_t ncols,const void *const*const values,struct _MySQLBindWrapper *wrapper);

#endif
