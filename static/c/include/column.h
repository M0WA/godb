#pragma once

#include "columntypes.h"

typedef struct _DBColumnDef {
	DBColumnType type;
	const char* name;
	const char* table;
	const char* database;
} DBColumnDef;
