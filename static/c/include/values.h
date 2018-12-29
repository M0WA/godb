#pragma once

#include <stddef.h>

struct _DBTable;
struct _DBColumnDef;
struct _StringBuf;

typedef int (*ValueSpecifier)(const struct _DBColumnDef *def,const void* value,struct _StringBuf *sql,size_t *serial);

int upsert_values_row_string(const struct _DBTable *tbl,ValueSpecifier spec,struct _StringBuf *sql,size_t *serial);
int insert_values_row_string(const struct _DBTable *tbl,ValueSpecifier spec,struct _StringBuf *sql,size_t *serial);
int update_values_string(const struct _DBTable *tbl,ValueSpecifier spec,struct _StringBuf *sql,size_t *serial);
int values_generic_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial);
