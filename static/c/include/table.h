#pragma once

#include <stddef.h>

struct _DBColumnDef;

typedef struct _DBTableDef {
	const char* name;
	const char* database;
	const struct _DBColumnDef* cols;
	size_t ncols;
} DBTableDef;

#pragma pack(push, 1)
typedef struct _DBTable {
	const struct _DBTableDef* def;
	void** valbuf;
} DBTable;
#pragma pack(pop)
