#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _WhereClause;

char* comma_concat_colnames(const struct _DBColumnDef *const cols,size_t ncols );
