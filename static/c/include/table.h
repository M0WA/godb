#pragma once

#include <stddef.h>

struct _DBColumnDef;

typedef struct _DBTableRow {
	void ***buf;
	int  **isset;
	size_t nrows;
} DBTableRow;

typedef struct _DBTableDef {
	const char *name;
	const char *database;
	const struct _DBColumnDef *cols;
	size_t ncols;
	const struct _UniqKey *uniquekeys;
	size_t nunique;
	const struct _DBColumnDef *primarykey;
} DBTableDef;

typedef struct _DBTable {
	const struct _DBTableDef *def;
	struct _DBTableRow rows;
} DBTable;

int create_dbtable(struct _DBTable *tbl, const struct _DBTableDef *def,size_t rows);
void destroy_dbtable(struct _DBTable *tbl);
void* get_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col);
void setnull_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col);
