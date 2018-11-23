#pragma once

#include <stddef.h>

struct _DBTable;

typedef struct _SelectResult {
	const struct _DBColumnDef *cols;
	size_t ncols;
	void** row;
} SelectResult;

int destroy_selectresult(SelectResult *res);
/*
int dump_selectresult(const SelectResult *res, char** buf);
*/
