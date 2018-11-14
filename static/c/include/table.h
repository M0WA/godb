#pragma once

#include <stddef.h>

struct _DBColumn;

typedef void* DBColumnValues;

typedef struct _DBTableDef {
	const char* name;
	const char* database;
	struct _DBColumnDef* cols;
	size_t ncols;
} DBTableDef;

typedef struct _DBTable {
	const struct _DBTableDef* def;
	DBColumnValues vals;
} DBTable;
