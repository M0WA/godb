#include "helper.h"

#include "column.h"

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
