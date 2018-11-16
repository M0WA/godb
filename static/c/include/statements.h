#pragma once

#include <stdint.h>

#include "table.h"

typedef struct _WhereClausePart {
} WhereClausePart;

typedef struct _WhereClause {
	struct _WhereClausePart* parts;
	size_t cnt;
} WhereClause;

typedef struct _InsertStmt {
	const struct _DBColumnDef* defs;
	size_t ncols;
	const unsigned char** valbuf;
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
