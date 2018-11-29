#pragma once

#ifndef DISABLE_POSTGRES

#include <stddef.h>
#include <postgresql/libpq-fe.h>

#include "dblimits.h"

struct _DBColumnDef;

typedef struct _PostgresParamWrapper {
	Oid types[MAX_BIND_COLS];
	const char *values[MAX_BIND_COLS];
	int lengths[MAX_BIND_COLS];
	int formats[MAX_BIND_COLS];
	int nparam;
} PostgresParamWrapper;

int postgres_param_append(const struct _DBColumnDef *def,size_t serial,const void *const val,PostgresParamWrapper *param);

#endif
