#pragma once

#ifdef _ENABLE_POSTGRESHELPER

#include <stddef.h>

#include "values.h"
#include "where.h"

struct _DBColumnDef;
struct _UpsertStmt;
struct tm;
struct _StringBuf;

int postgres_time_to_tm(const char *, struct tm *);
int postgres_upsert_stmt_string(const struct _UpsertStmt *const, ValueSpecifier, struct _StringBuf*);

#endif
