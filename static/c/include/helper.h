#pragma once

#include <stddef.h>

struct _DBColumnDef;
struct _WhereClause;

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

char* comma_concat_colnames(const struct _DBColumnDef *const cols,size_t ncols,int skip_autoincrement);
int append_string(const char *src, char** dest);
void get_limit(const size_t limits[], char *limit);
