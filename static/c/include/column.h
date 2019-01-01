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
	int notsigned;
	size_t size;
} DBColumnDef;

struct tm;
struct _StringBuf;

typedef int(*StringToTm)(const char*,struct tm*);

void dump_dbcolumndef(const struct _DBColumnDef *def, const char* prefix, struct _StringBuf *buf);
size_t get_column_bufsize(const struct _DBColumnDef *def);
int get_column_string(char *colstr,size_t colen,const struct _DBColumnDef *def,const void *buf);
int set_columnbuf_by_string(const struct _DBColumnDef *def,StringToTm conv,void *buf,const char *val);
