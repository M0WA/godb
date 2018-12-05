#pragma once

#include <stdint.h>

#include "table.h"
#include "where.h"
#include "values.h"
#include "aggregate.h"
#include "order.h"

struct _UniqKey;
struct _StringBuf;

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
	struct _GroupByClause groupby;
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
	const struct _DBColumnDef *defs;
	size_t ncols;
	const struct _UniqKey *const *uniqs;
	size_t nuniq;
	const struct _DBColumnDef *prikey;
	const void *const*const* valbuf;
	size_t nrows;
} UpsertStmt;

int update_stmt_string(const UpdateStmt *const, ValueSpecifier val, WhereSpecifier where, struct _StringBuf *sql, int skip_autoincrement);
int delete_stmt_string(const DeleteStmt *const, WhereSpecifier where, struct _StringBuf *sql);
int select_stmt_string(const SelectStmt *const, WhereSpecifier where, struct _StringBuf *sql);
int insert_stmt_string(const InsertStmt *const, ValueSpecifier val, struct _StringBuf *sql, int skip_autoincrement);
