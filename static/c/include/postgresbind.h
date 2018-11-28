#pragma once

#ifndef DISABLE_POSTGRES

#include <stddef.h>
#include <postgresql/libpq-fe.h>

typedef struct _PostgresParamWrapper {
	Oid *types;
	const char * const *values;
	const int *lengths;
	const int *formats;
	int nparam;
} PostgresParamWrapper;

int postgres_append_param(const DBColumnDef *def,const void *const val,size_t serial,PostgresParamWrapper *param);

#endif
