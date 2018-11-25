#pragma once

#ifndef _DISABLE_POSTGRES

#include <stddef.h>

struct _DBColumnDef;
struct tm;

int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial);
int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial);
int postgres_time_to_tm(const char *val, struct tm *t);

#endif
