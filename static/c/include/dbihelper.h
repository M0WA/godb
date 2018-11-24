#ifndef _DISABLE_DBI

#include <stddef.h>

struct _DBColumnDef;

int dbi_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial);
int dbi_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial);

#endif
