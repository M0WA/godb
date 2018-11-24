#pragma once

#include <stddef.h>

struct _DBColumnDef;

typedef int (*ValueSpecifier)(const struct _DBColumnDef *def,const void* value,char** sql,size_t *serial);

int values_string(const struct _DBColumnDef *def, size_t ncols,ValueSpecifier spec, const void* const*const values,char** sql,size_t *serial,int skip_autoincrement);
int values_row_string(const struct _DBColumnDef *def, size_t ncols,ValueSpecifier spec, const void * const* const* const values, size_t nrows,char** sql,size_t *serial,int skip_autoincrement);
