#pragma once

#include <stddef.h>

struct _DBTable;

typedef struct _SelectResult {
	const struct _DBColumnDef *cols;
	size_t ncols;
	void** rows;
	size_t nrows;
} SelectResult;

int destroy_selectresult();
