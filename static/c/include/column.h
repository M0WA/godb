#pragma once

#include <stddef.h>

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

const void* get_columnbuf_from_row(const struct _DBColumnDef *def,size_t colidx,const void *buf);
size_t get_column_bufsize(const struct _DBColumnDef *def);
int get_column_string(char *colstr,size_t colen,const struct _DBColumnDef *def,const void *buf);
