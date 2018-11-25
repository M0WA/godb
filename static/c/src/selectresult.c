#include "selectresult.h"

#include <stdlib.h>

#include "helper.h"
#include "table.h"
#include "column.h"
#include "logger.h"
#include "selectresult.h"

int dump_selectresult(const SelectResult *res, char** buf) {
	*buf = 0;
	size_t bufsize = 0;
	char *tmpbuf = 0;

	if(append_string(" | ",buf)) {
		return 1; }
	for(size_t i = 0; i < res->ncols; i++) {
		size_t oldbufsize = bufsize;
		bufsize = res->cols[i].type == COL_TYPE_STRING ? res->cols[i].size : 64;
		if(bufsize > oldbufsize) {
			tmpbuf = alloca(bufsize);
			oldbufsize = bufsize; }
		if(!tmpbuf) {
			return 1; }
		if( get_column_string(tmpbuf,bufsize,&(res->cols[i]),res->row[i]) ) {
			return 1; }
		if(append_string(tmpbuf,buf)) {
			return 1; }
		if(append_string(" | ",buf)) {
			return 1; }
	}
	return 0;
}

int create_selectresult(const struct _DBColumnDef *defs,size_t ncols, struct _SelectResult *res) {
	if(!defs || !ncols || !res) {
		return 1; }

	res->cols = defs;
	res->ncols = ncols;
	res->row = calloc(sizeof(void*),res->ncols);
	for(size_t i = 0; i < res->ncols; i++) {
		size_t colsize = get_column_bufsize(&(defs[i]));
		if(colsize == 0) {
			free(res->row);
			res->row = 0;
			LOG_WARN("invalid column size");
			return 1; }
		res->row[i] = calloc(colsize,1);
	}
	return 0;
}

int destroy_selectresult(struct _SelectResult *res) {
	if(res) {
		if(res->row) {
			for(size_t i = 0; i < res->ncols; i++) {
				if(res->row[i]) {
					free(res->row[i]);
					res->row[i] = 0;
				}
			}
			free(res->row);
		}
		res->row = 0;
	}
	return 0;
}
