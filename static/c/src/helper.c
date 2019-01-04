#include "helper.h"

#include "column.h"
#include "table.h"
#include "stringbuf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

int comma_concat_colnames_setonly(struct _StringBuf *buf,const struct _DBTable *tbl,const char *delimiter) {
	const char *del = delimiter ? delimiter : "";
	size_t p = 0;

	for(size_t i = 0; i < tbl->def->ncols; i++) {
		if(!tbl->rows.isset[0][i]) {
			continue; }
		if( stringbuf_appendf(buf,"%s%s%s%s",(p ? "," : ""),del,tbl->def->cols[i].name,del) ) {
			return 1;
		}
		p++;
	}
	return 0;
}

int comma_concat_colnames_select(struct _StringBuf *buf,const struct _DBColumnDef *const cols,size_t ncols,const char *delimiter) {
	const char *del = delimiter ? delimiter : "";
	for(size_t i = 0; i < ncols; i++) {
		if( stringbuf_appendf(buf,"%s%s%s%s.%s%s%s",(i ? "," : ""),del,cols[i].table,del,del,cols[i].name,del) ) {
			return 1;
		}
	}
	return 0;
}

int comma_concat_colnames_insert(struct _StringBuf *buf,const struct _DBColumnDef *const cols,size_t ncols,const char *delimiter) {
	const char *del = delimiter ? delimiter : "";
	for(size_t i = 0; i < ncols; i++) {
		if( stringbuf_appendf(buf,"%s%s%s%s",(i ? "," : ""),del,cols[i].name,del) ) {
			return 1;
		}
	}
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
