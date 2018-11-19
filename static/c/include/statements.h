#pragma once

#include <stdint.h>

#include "table.h"
#include "where.h"

typedef struct _OrderBy {
} OrderBy;

typedef struct _InsertStmt {
	const struct _DBColumnDef * defs;
	size_t ncols;
	const void *const*const* valbuf;
	size_t nrows;
} InsertStmt;

typedef struct _SelectStmt {
	const struct _DBColumnDef* defs;
	size_t ncols;
	struct _WhereClause where;
	const struct _DBColumnDef* groupby;
	size_t ngroups;
	size_t limit;
} SelectStmt;

typedef struct _UpdateStmt {
} UpdateStmt;

typedef struct _DeleteStmt {
} DeleteStmt;

typedef struct _UpsertStmt {
} UpsertStmt;
