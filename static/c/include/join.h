#pragma once

#include <stddef.h>

typedef enum _DBJoinType {
	DBJOIN_TYPE_INVALID = 0,

	DBJOIN_TYPE_LEFT,
	DBJOIN_TYPE_RIGHT,
	DBJOIN_TYPE_INNER,
	DBJOIN_TYPE_OUTER,

	DBJOIN_TYPE_MAX,
} DBJoinType;

struct _DBTableDef;
struct _DBColumnDef;
struct _StringBuf;
struct _UpdateStmt;
struct _SelectStmt;

typedef struct _JoinStmt {
	DBJoinType type;
	const struct _DBColumnDef *left;
	const struct _DBColumnDef *right;
} JoinStmt;

typedef struct _JoinClause {
	struct _JoinStmt **joins;
	size_t njoins;
} JoinClause;

int join_clause_string(const struct _JoinClause *join,const char *delimiter,struct _StringBuf *buf);
int join_append(struct _JoinClause *join, struct _JoinStmt *stmt);
void join_destroy(struct _JoinClause *join);
