#pragma once

struct _DBColumnDef;

typedef enum _DBOrderType {
	DBORDER_TYPE_INVALID = 0,

	DBORDER_TYPE_ASCENDING,
	DBORDER_TYPE_DECENDING,

	DBORDER_TYPE_MAX,
} DBOrderType;

typedef struct _OrderByClause {
	DBOrderType direction;
	const struct _DBColumnDef *col;
} OrderByClause;
