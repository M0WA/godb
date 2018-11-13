#pragma once

typedef enum _DBColumnType {
	COL_TYPE_STRING,
	COL_TYPE_INT,
	COL_TYPE_FLOAT,
	COL_TYPE_DATETIME
} DBColumnType;

typedef struct _DBColumnDef {
	DBColumnType type;
	const char* name;
	const char* table;
	const char* database;
} DBColumnDef;
