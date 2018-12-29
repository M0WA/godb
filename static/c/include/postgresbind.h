#pragma once

#ifndef _DISABLE_POSTGRES

#include <stddef.h>
#include <postgresql/libpq-fe.h>

#include "dblimits.h"

struct _DBColumnDef;
struct _WhereClause;
struct _StringBuf;
struct _DBTable;

typedef struct _PostgresParamWrapper {
	Oid types[MAX_BIND_COLS];
	const char *values[MAX_BIND_COLS];
	int lengths[MAX_BIND_COLS];
	int formats[MAX_BIND_COLS];
	char buf[MAX_BIND_COLS][POSTGRES_BIND_BUF];
	int nparam;
} PostgresParamWrapper;

int postgres_where(const struct _WhereClause *clause,PostgresParamWrapper *param);
int postgres_param_append(const struct _DBColumnDef *def,const void *const val, int isset,PostgresParamWrapper *param);
int postgres_values(const struct _DBTable *tbl, size_t row,PostgresParamWrapper *param);
int postgres_update_values(const struct _DBTable *tbl,size_t row,PostgresParamWrapper *param);
int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t* serial);
int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial);

#endif
