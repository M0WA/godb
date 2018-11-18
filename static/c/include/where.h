#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _WhereClause;

typedef enum _WhereCondOp {
	WHERE_EQUAL,
	WHERE_NOT_EQUAL,
	WHERE_LIKE,
	WHERE_NOT_LIKE,
} WhereCondOperator;

typedef enum _WhereCompOp {
	WHERE_AND,
	WHERE_OR,
} WhereCompOperator;

typedef enum _WhereEntryType {
	WHERE_COND,
	WHERE_COMP,
} WhereEntryType;

typedef struct _WhereCondition {
	WhereEntryType type;
	WhereCompOperator comp;
	WhereCondOperator op;
	const struct _DBColumnDef* def;
	size_t colcnt;
	const void* values;
	size_t valcnt;
} WhereCondition;

typedef struct _WhereComposite {
	WhereEntryType type;
	WhereCompOperator op;
	struct _WhereClause** where;
	size_t cnt;
} WhereComposite;

typedef union _WhereStmt {
	WhereComposite comp;
	WhereCondition cond;
} WhereStmt;

typedef struct _WhereClause {
	WhereStmt** stmts;
	size_t cnt;
} WhereClause;

void where_destroy(struct _WhereClause* c);
int where_comp_append(struct _WhereComposite* comp,struct _WhereClause* clause);
int where_append(struct _WhereClause* clause,union _WhereStmt* stmt);
