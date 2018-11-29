#pragma once

#ifdef _ENABLE_POSTGRESHELPER

#include <stddef.h>

struct _DBColumnDef;
struct _UpsertStmt;
struct tm;

int postgres_time_to_tm(const char *val, struct tm *t);
int postgres_upsert_stmt_string(const struct _UpsertStmt *const, char**);

#endif
