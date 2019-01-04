#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _DBTable;
struct _WhereClause;
struct _StringBuf;

#define ARRAY_RESIZE(arr,idx,type) ({ \
	int rc = 0; \
	if(arr) { \
		typeof (arr) tmp = realloc(arr,(sizeof(type) * idx) + 1); \
		if(tmp) { \
			arr = tmp; } \
		else { \
			rc = 1;	} \
	} else { \
		arr = malloc(sizeof(type)); \
		if(!arr) { \
			rc = 1; } \
	} \
	rc; \
})

int comma_concat_colnames_select(struct _StringBuf *buf,const struct _DBColumnDef *const cols,size_t ncol,const char *delimiter);
int comma_concat_colnames_insert(struct _StringBuf *buf,const struct _DBColumnDef *const cols,size_t ncol,const char *delimiter);
int comma_concat_colnames_setonly(struct _StringBuf *buf,const struct _DBTable *tbl,const char *delimiter);
int append_string(const char *src, char** dest);
void get_limit(const size_t limits[], char *limit);
