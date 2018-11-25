#pragma once

#include <stdint.h>

#include "table.h"
#include "where.h"
#include "values.h"

#define DESTROY_STMT(somestmt) where_destroy(&((somestmt)->where));

typedef struct _OrderBy {
} OrderBy;

typedef struct _InsertStmt {
	const struct _DBColumnDef *defs;
	size_t ncols;
	const void *const*const* valbuf;
	size_t nrows;
} InsertStmt;

typedef struct _SelectStmt {
	const struct _DBColumnDef *defs;
	size_t ncols;
	struct _WhereClause where;
	const struct _DBColumnDef *groupby;
	size_t ngroups;
	size_t limit[2];
} SelectStmt;

typedef struct _DeleteStmt {
	const struct _DBTableDef *def;
	struct _WhereClause where;
} DeleteStmt;

typedef struct _UpdateStmt {
	const struct _DBColumnDef *defs;
	size_t ncols;
	const void *const* valbuf;
	struct _WhereClause where;
} UpdateStmt;

typedef struct _UpsertStmt {
} UpsertStmt;

int update_stmt_string(const UpdateStmt *const, ValueSpecifier val, WhereSpecifier where, char** sql, int skip_autoincrement);
int delete_stmt_string(const DeleteStmt *const, WhereSpecifier where, char** sql);
int select_stmt_string(const SelectStmt *const, WhereSpecifier where, char** sql);
int insert_stmt_string(const InsertStmt *const, ValueSpecifier val, char** sql, int skip_autoincrement);
