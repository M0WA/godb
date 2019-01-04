#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _DBTableDef;
struct _StringBuf;

#include "table.h"

typedef struct _SelectResult {
	struct _DBTable tbl;
} SelectResult;

int destroy_selectresult(SelectResult *res);
int create_selectresult(const struct _DBTableDef *def, struct _SelectResult *res);
int dump_selectresult(const SelectResult *res, struct _StringBuf *buf);
