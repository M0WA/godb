#pragma once

#ifndef _DISABLE_POSTGRES

#include <stddef.h>
#include <postgresql/libpq-fe.h>

#include "dblimits.h"

struct _DBColumnDef;
struct _WhereClause;

typedef struct _PostgresParamWrapper {
	Oid types[MAX_BIND_COLS];
	const char *values[MAX_BIND_COLS];
	int lengths[MAX_BIND_COLS];
	int formats[MAX_BIND_COLS];
	char buf[MAX_BIND_COLS][POSTGRES_BIND_BUF];
	int nparam;
} PostgresParamWrapper;

int postgres_where(const struct _WhereClause *clause,PostgresParamWrapper *param);
int postgres_param_append(const struct _DBColumnDef *def,const void *const val,PostgresParamWrapper *param);
int postgres_values(const struct _DBColumnDef *defs,size_t ncols,const void *const*const values,PostgresParamWrapper *param);
int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial);
int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial);

#endif
