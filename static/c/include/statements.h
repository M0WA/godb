#pragma once

#include <stdint.h>

#include "table.h"
#include "where.h"

typedef struct _InsertStmt {
	const struct _DBColumnDef * defs;
	size_t ncols;
	const void *const*const* valbuf;
	size_t nrows;
} InsertStmt;

typedef struct _UpdateStmt {
	struct _WhereClause where;
} UpdateStmt;

typedef struct _UpsertStmt {
} UpsertStmt;

typedef struct _DeleteStmt {
	struct _WhereClause where;
} DeleteStmt;

typedef struct _SelectStmt {
	struct _WhereClause where;
} SelectStmt;
