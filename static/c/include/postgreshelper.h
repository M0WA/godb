#pragma once

#ifdef _ENABLE_POSTGRESHELPER

#include <stddef.h>

#include "values.h"
#include "where.h"

struct _DBColumnDef;
struct _UpsertStmt;
struct tm;
struct _StringBuf;

int postgres_raw_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial);
int postgres_time_to_tm(const char *, struct tm *);
int postgres_upsert_stmt_string(const struct _UpsertStmt *const, ValueSpecifier, const char *delimiter, struct _StringBuf*);

#endif
