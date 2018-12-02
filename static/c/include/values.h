#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _StringBuf;

typedef int (*ValueSpecifier)(const struct _DBColumnDef *def,const void* value,struct _StringBuf *sql,size_t *serial);

int insert_values_row_string(const struct _DBColumnDef *def, size_t ncols,ValueSpecifier spec, const void * const* const* const values, size_t nrows,struct _StringBuf *sql,size_t *serial,int skip_autoincrement);
int update_values_string(const struct _DBColumnDef *def, size_t ncols,ValueSpecifier spec, const void * const* values,struct _StringBuf *sql,size_t *serial,int skip_autoincrement);
int values_generic_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial);
