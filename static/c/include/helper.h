#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _DBTable;
struct _WhereClause;
struct _StringBuf;

int comma_concat_colnames_select(struct _StringBuf *buf,const struct _DBColumnDef *const cols,size_t ncol,const char *delimiter);
int comma_concat_colnames_insert(struct _StringBuf *buf,const struct _DBColumnDef *const cols,size_t ncol,const char *delimiter);
int comma_concat_colnames_setonly(struct _StringBuf *buf,const struct _DBTable *tbl,const char *delimiter);
void get_limit(const size_t limits[], char *limit);
