#ifndef _DISABLE_DBI

#include <stddef.h>

struct _DBColumnDef;
struct _DBHandle;
struct _UpsertStmt;

int dbi_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial);
int dbi_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial);
int dbi_upsert_stmt_string(const struct _DBHandle *dbh,const struct _UpsertStmt *const s, char** sql);

#endif
