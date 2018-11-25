#pragma once

#ifdef _ENABLE_POSTGRESHELPER

#include <stddef.h>

struct _DBColumnDef;
struct _UpsertStmt;
struct tm;

int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial);
int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial);
int postgres_time_to_tm(const char *val, struct tm *t);
int postgres_upsert_stmt_string(const struct _UpsertStmt *const, char**);

#endif
