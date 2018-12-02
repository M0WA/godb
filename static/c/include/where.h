#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _WhereClause;
struct _StringBuf;

typedef enum _WhereCondOp {
	WHERE_EQUAL,
	WHERE_NOT_EQUAL,
	WHERE_LIKE,
	WHERE_NOT_LIKE,
	WHERE_IS_NULL,
	WHERE_IS_NOT_NULL,
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
	WhereCondOperator cond;
	const struct _DBColumnDef *def;
	const void** values;
	size_t cnt;
} WhereCondition;

typedef struct _WhereComposite {
	WhereEntryType type;
	struct _WhereClause** where;
	size_t cnt;
} WhereComposite;

typedef union _WhereStmt {
	WhereComposite comp;
	WhereCondition cond;
} WhereStmt;

typedef struct _WhereClause {
	WhereCompOperator comp;
	WhereStmt** stmts;
	size_t cnt;
} WhereClause;

typedef int (*WhereSpecifier)(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial);

void where_destroy(struct _WhereClause *c);
int where_comp_append(struct _WhereComposite *comp,struct _WhereClause *clause);
int where_append(struct _WhereClause *clause,union _WhereStmt *stmt);
int where_string(const struct _WhereClause *c,WhereSpecifier spec,struct _StringBuf *sql, size_t *serial);
int where_generic_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t* serial);
