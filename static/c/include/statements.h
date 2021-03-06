#pragma once

#include <stdint.h>

#include "table.h"
#include "where.h"
#include "values.h"
#include "aggregate.h"
#include "order.h"
#include "join.h"

struct _UniqKey;
struct _StringBuf;
struct _SelectResult;

typedef struct _InsertStmt {
	const struct _DBTable *dbtbl;
	struct _JoinClause *joins;
	size_t njoins;
} InsertStmt;

typedef struct _SelectStmt {
	const struct _DBTableDef *from;
	struct _DBTableDef select;
	struct _WhereClause where;
	struct _GroupByClause groupby;
	struct _JoinClause joins;
	size_t limit[2];
} SelectStmt;

typedef struct _DeleteStmt {
	const struct _DBTableDef *def;
	struct _WhereClause where;
} DeleteStmt;

typedef struct _UpdateStmt {
	const struct _DBTable *dbtbl;
	struct _JoinClause joins;
	struct _WhereClause where;
} UpdateStmt;

typedef struct _UpsertStmt {
	const struct _DBTable *dbtbl;
} UpsertStmt;

int create_insert_stmt(struct _InsertStmt *s,const struct _DBTable *dbtbl);
int create_select_stmt(struct _SelectStmt *s,const struct _DBTableDef *def);
int create_delete_stmt(struct _DeleteStmt *s,const struct _DBTableDef *def);
int create_upsert_stmt(struct _UpsertStmt *s,const struct _DBTable *dbtbl);
int create_update_stmt(struct _UpdateStmt *s,const struct _DBTable *dbtbl);

void destroy_select_stmt(struct _SelectStmt *s);
void destroy_delete_stmt(struct _DeleteStmt *s);
void destroy_update_stmt(struct _UpdateStmt *s);

int update_stmt_string(const UpdateStmt *const, ValueSpecifier val, WhereSpecifier where, const char *delimiter, struct _StringBuf *sql);
int delete_stmt_string(const DeleteStmt *const, WhereSpecifier where, const char *delimiter, struct _StringBuf *sql);
int select_stmt_string(const SelectStmt *const, const struct _SelectResult *const, WhereSpecifier where, const char *delimiter, struct _StringBuf *sql);
int insert_stmt_string(const InsertStmt *const, ValueSpecifier val, const char *delimiter, struct _StringBuf *sql);
