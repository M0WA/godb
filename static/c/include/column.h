#pragma once

#include <stdint.h>
#include "columntypes.h"

typedef struct _DBColumnDef {
	DBColumnType type;
	const char *name;
	const char *table;
	const char *database;
	int autoincrement;
	int notnull;
	size_t size;
} DBColumnDef;
