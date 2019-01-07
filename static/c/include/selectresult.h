#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _DBTableDef;
struct _StringBuf;
struct _SelectStmt;

#include "table.h"

typedef struct _SelectResult {
	struct _DBTable tbl;
	struct _DBTableDef *def;
	struct _DBColumnDef *cols;
	size_t ncols;
} SelectResult;

int destroy_selectresult(SelectResult *res);
int create_selectresult(const struct _SelectStmt *stmt, struct _SelectResult *res);
int dump_selectresult(const SelectResult *res, struct _StringBuf *buf);
