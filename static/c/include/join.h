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

typedef struct _JoinClause {
	DBJoinType type;
	const struct _DBTableDef  *left;
	const struct _DBColumnDef *leftcol;
	const struct _DBTableDef  *right;
	const struct _DBColumnDef *rightcol;
} JoinClause;
