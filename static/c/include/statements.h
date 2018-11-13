#pragma once

#include <stdint.h>

typedef struct _InsertStmt {
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

typedef struct _WhereClausePart {
} WhereClausePart;

typedef struct _WhereClause {
	struct _WhereClausePart* parts;
	size_t cnt;
} WhereClause;
