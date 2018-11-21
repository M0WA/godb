#include "helper.h"

#include "column.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* comma_concat_colnames(const struct _DBColumnDef *const cols,size_t ncols ) {
	size_t string_size = 0;
	char* buf = 0;
	for(size_t i = 0; i < ncols; i++) {
		string_size += strlen(cols[i].name);
		string_size++; //comma
	}
	string_size++; //zero-terminate

	buf = malloc(string_size);
	if(!buf) {
		return 0; }
	buf[0] = 0;

	for(size_t i = 0; i < ncols; i++) {
		if(i != 0) {
			strcat(buf,","); }
		strcat(buf,cols[i].name);
	}
	return buf;
}

int append_string(const char *src, char** dest) {
	if(!src || !dest) {
		return 1; }

	size_t newsize = 1;
	if(!*dest) {
		newsize += strlen(src);
		*dest = malloc(sizeof(newsize));
		if(!*dest) {
			return 1; }
		*(dest[0]) = 0;
	} else {
		char *tmp = 0;
		newsize += strlen(src) + strlen(*dest);
		tmp = realloc((*dest),newsize);
		if(!tmp) {
			return 1; }
		*dest = tmp;
	}
	strcat(*dest,src);
	return 0;
}

void get_limit(const size_t limits[], char *limit) {
	limit[0] = 0;
	if(limits[0] > 0) {
		snprintf(limit,32,"LIMIT %lu",limits[0]);
		if( limits[1] ) {
			snprintf(limit,32,"LIMIT %lu %lu",limits[0],limits[1]); }
	}
}
