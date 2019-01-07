#include "selectresult.h"

#include "helper.h"
#include "table.h"
#include "column.h"
#include "logger.h"
#include "selectresult.h"
#include "stringbuf.h"
#include "statements.h"
#include "tables.h"

#include <stdlib.h>
#include <alloca.h>
#include <string.h>

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

int create_selectresult(const struct _SelectStmt *stmt, struct _SelectResult *res) {
	if(!stmt || !res) {
		return 1; }

	res->def = calloc(1,sizeof(DBTableDef));
	*(res->def) = *(stmt->from);
	if(!stmt->select.ncols|| !stmt->select.cols) {
		// calculate amount of columns to allocate
		size_t ncols = stmt->from->ncols;
		for(size_t i = 0; i < stmt->joins.njoins; i++) {
			if(!stmt->joins.joins[i] || !stmt->joins.joins[i]->right) {
				return 1; }
			const DBTableDef *tbldef = get_table_def_by_name(stmt->joins.joins[i]->right->database, stmt->joins.joins[i]->right->table);
			if(!tbldef) {
				return 1; }
			ncols += tbldef->ncols;
		}

		res->cols = calloc(ncols,sizeof(DBColumnDef));
		size_t pos = 0;
		for(size_t i = 0; i < stmt->from->ncols; i++,pos++) {
			memcpy(&res->cols[pos],&stmt->from->cols[i],sizeof(DBColumnDef));
		}
		for(size_t i = 0; i < stmt->joins.njoins; i++) {
			const DBTableDef *tbldef = get_table_def_by_name(stmt->joins.joins[i]->right->database, stmt->joins.joins[i]->right->table);
			memcpy(&res->cols[pos],tbldef->cols,sizeof(DBColumnDef) * tbldef->ncols);
			pos += tbldef->ncols;
		}

		res->def->cols = res->cols;
		res->def->ncols = ncols;
	} else {
		res->def->cols = stmt->select.cols;
		res->def->ncols = stmt->select.ncols;
	}

	if( create_dbtable(&(res->tbl), res->def, 1) ) {
		return 1; }

	return 0;
}

int destroy_selectresult(struct _SelectResult *res) {
	if(res) {
		destroy_dbtable(&res->tbl);
		if(res->def) {
			free(res->def);
		}
		if(res->cols) {
			free(res->cols);
		}
	}
	return 0;
}
