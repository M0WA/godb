#pragma once

#include <stddef.h>

struct _DBColumnDef;

typedef struct _DBGroupByClause {
	const struct _DBColumnDef *cols;
	size_t ncols;
} DBGroupByClause;

typedef struct _DBSumAggregate {
	const struct _DBColumnDef *col;
} DBSumAggregate;

typedef struct _DBAverageAggregate {
	const struct _DBColumnDef *col;
} DBAverageAggregate;
