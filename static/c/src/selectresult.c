#include "selectresult.h"

#include "helper.h"
#include "table.h"
#include "column.h"
#include "logger.h"
#include "selectresult.h"
#include "stringbuf.h"

#include <stdlib.h>
#include <alloca.h>

int dump_selectresult(const SelectResult *res, struct _StringBuf *buf) {
	size_t bufsize = 0;
	char *tmpbuf = 0;
	if(stringbuf_append(buf," | ")) {
		return 1; }
	for(size_t i = 0; i < res->tbl.def->ncols; i++) {
		size_t oldbufsize = bufsize;
		bufsize = res->tbl.def->cols[i].type == COL_TYPE_STRING ? res->tbl.def->cols[i].size : 64;
		if(bufsize > oldbufsize) {
			tmpbuf = alloca(bufsize);
			oldbufsize = bufsize; }
		if(!tmpbuf) {
			return 1; }
		if( get_column_string(tmpbuf,bufsize,&(res->tbl.def->cols[i]),res->tbl.rows.buf[0][i]) ) {
			return 1; }
		if(stringbuf_appendf(buf,"%s | ",tmpbuf)) {
			return 1; }

	}
	return 0;
}

int create_selectresult(const struct _DBTableDef *def, struct _SelectResult *res) {
	if(!def || !res) {
		return 1; }

	if( create_dbtable(&(res->tbl), def, 1) ) {
		return 1; }

	return 0;
}

int destroy_selectresult(struct _SelectResult *res) {
	if(res) {
		destroy_dbtable(&res->tbl);
	}
	return 0;
}
