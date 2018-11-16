#pragma once

#include <stddef.h>

struct _DBColumnDef;

typedef struct _DBTableDef {
	const char* name;
	const char* database;
	const struct _DBColumnDef* cols;
	size_t ncols;
} DBTableDef;

typedef struct _DBTable {
	const struct _DBTableDef* def;
	const size_t* bytepos;
	unsigned char* valbuf;
} DBTable;

int get_colidx_by_name(const char* colname,const struct _DBTableDef* def,size_t* idx);
const void* get_colbuf_by_idx(size_t idx,const struct _DBTable* tbl);
