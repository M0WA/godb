#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _StringBuf;

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
	const struct _ForeignKey *foreignkeys;
	size_t nforeign;
	const struct _IndexKey *indexkeys;
	size_t nindex;
	const struct _UniqKey *uniquekeys;
	size_t nunique;
	const struct _DBColumnDef *primarykey;
} DBTableDef;

typedef struct _DBTable {
	const struct _DBTableDef *def;
	struct _DBTableRow rows;
} DBTable;

void dump_dbtabledef(const struct _DBTableDef *def, struct _StringBuf *buf);
int create_dbtable(struct _DBTable *tbl, const struct _DBTableDef *def,size_t rows);
void destroy_dbtable(struct _DBTable *tbl);
const void* get_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col);
void* set_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col);
void setnull_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col);
