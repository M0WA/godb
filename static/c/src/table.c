#include "table.h"

#include <stdlib.h>

#include "column.h"
#include "stringbuf.h"
#include "keys.h"

int create_dbtable(struct _DBTable *tbl, const struct _DBTableDef *def,size_t rows) {
	if(!tbl || !rows) {
		return 1; }

	tbl->def = def;

	tbl->rows.buf = calloc(rows,sizeof(void*));
	tbl->rows.isset = calloc(rows,sizeof(int*));
	for(size_t row = 0; row < rows; row++) {
		tbl->rows.buf[row] = calloc(tbl->def->ncols,sizeof(void*));
		tbl->rows.isset[row] = calloc(tbl->def->ncols,sizeof(int));
		if(!tbl->rows.buf[row]) {
			return 1;
		}
/*
		for(size_t col = 0; col < tbl->def->ncols; col++) {
			tbl->rows.buf[row][col] = calloc(1,get_column_bufsize(&(tbl->def->cols[col])));
			if(!tbl->rows.buf[row][col]) {
				return 1;
			}
		}
*/
	}
	tbl->rows.nrows = rows;
	return 0;
}

void destroy_dbtable(struct _DBTable *tbl) {
	if(!tbl) {
		return;
	}
	for(size_t row = 0; row < tbl->rows.nrows; row++) {
		for(size_t col = 0; col < tbl->def->ncols; col++) {
			if(tbl->rows.buf[row][col]) {
				free(tbl->rows.buf[row][col]);
			}
		}
		if(tbl->rows.isset && tbl->rows.isset[row]) {
			free(tbl->rows.isset[row]);
		}
		if(tbl->rows.buf && tbl->rows.buf[row]) {
			free(tbl->rows.buf[row]);
		}
	}
	if(tbl->rows.isset) {
		free(tbl->rows.isset);
	}
	if(tbl->rows.buf) {
		free(tbl->rows.buf);
	}
}

const void* get_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col) {
	return tbl->rows.buf[row][col];
}

void* set_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col) {
	if(!tbl->rows.buf[row][col]) {
		tbl->rows.buf[row][col] = malloc(get_column_bufsize(&tbl->def->cols[col]));
		if(!tbl->rows.buf[row][col]) {
			return 0;
		}
	}
	tbl->rows.isset[row][col] = 1;
	return tbl->rows.buf[row][col];
}

void setnull_dbtable_columnbuf(struct _DBTable *tbl, size_t row, size_t col) {
	if(tbl->rows.buf[row][col]) {
		free(tbl->rows.buf[row][col]);
		tbl->rows.buf[row][col] = 0;
	}
}

void dump_dbtabledef(const struct _DBTableDef *def, struct _StringBuf *buf) {
	stringbuf_append(buf,"\n========================================\n");
	stringbuf_appendf(buf,"\t%s.%s\n",def->database,def->name);
	stringbuf_append(buf,"========================================\n");

	stringbuf_append(buf,"columns:\n");
	for(size_t col = 0; col < def->ncols; col++) {
		dump_dbcolumndef(&(def->cols[col]),"\t", buf);
	}

	stringbuf_append(buf,"primary key:\n");
	dump_dbcolumndef(def->primarykey,"\t", buf);

	if(def->nindex) {
		stringbuf_append(buf,"index keys:\n");
		for(size_t idx = 0; idx < def->nindex; idx++) {
			dump_dbcolumndef(def->indexkeys[idx].col,"\t", buf);
		}
	}

	if(def->nunique) {
		stringbuf_append(buf,"unique keys:\n");
		for(size_t uniq = 0; uniq < def->nunique; uniq++) {
			stringbuf_appendf(buf,"\t%zu:\n",uniq);
			for(size_t col = 0; col < def->uniquekeys[uniq].ncols; col++) {
				dump_dbcolumndef(def->uniquekeys[uniq].cols[col],"\t\t", buf);
			}
		}
	}

	if(def->nforeign) {
		stringbuf_append(buf,"foreign keys:\n");
		for(size_t col = 0; col < def->nforeign; col++) {
			dump_dbcolumndef(def->foreignkeys[col].col,"\t", buf);
			stringbuf_append(buf,"\t\treferences:\n");
			dump_dbcolumndef(def->foreignkeys[col].ref,"\t\t\t", buf);
		}
	}
}
