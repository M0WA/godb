#pragma once

#include <stddef.h>

struct _DBColumnDef;

typedef enum _DBAggregateType {
	DBAGGREGATE_INVALID = 0,

	DBAGGREGATE_SUM,
	DBAGGREGATE_AVG,

	DBAGGREGATE_MAX,
} DBAggregateType;

typedef struct _GroupByClause {
	const struct _DBColumnDef *cols;
	size_t ncols;
} GroupByClause;
