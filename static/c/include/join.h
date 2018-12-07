#pragma once

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

typedef struct _JoinClause {
	DBJoinType type;
	const struct _DBTableDef  *left;
	const struct _DBColumnDef *leftcol;
	const struct _DBTableDef  *right;
	const struct _DBColumnDef *rightcol;
} JoinClause;

int join_clause_string(const struct _JoinClause *join,struct _StringBuf *buf);
