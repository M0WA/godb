#pragma once

#ifdef _ENABLE_MYSQLHELPER

#include "values.h"

#include <mysql/mysql.h>

struct _DBColumnDef;
struct _SelectResult;
struct _UpsertStmt;
struct _StringBuf;
struct tm;

int mysql_tm(const MYSQL_TIME *mt, struct tm *const t);
size_t mysql_get_colbuf_size(const struct _DBColumnDef *const col);
int mysql_datatype(const struct _DBColumnDef *const col,enum enum_field_types *ft);
int mysql_string_to_tm(const char *val, struct tm *t);
int mysql_upsert_stmt_string(const struct _UpsertStmt *const, ValueSpecifier,struct _StringBuf*);

#endif
