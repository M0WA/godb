#pragma once

struct _DBColumn;

typedef void* DBColumnValues

typedef struct _DBTableDef {
	const char* name;
	const char* database;
} DBTableDef;

typedef struct _DBTable {
	const struct _DBTableDef* def;
	const struct _DBColumnDef* cols;
	DBColumnValues vals;
	size_t ncols;
} DBTable;
