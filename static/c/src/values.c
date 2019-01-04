#include "values.h"

#include "column.h"
#include "helper.h"
#include "stringbuf.h"
#include "table.h"
#include "values.h"

#include <alloca.h>
#include <stdio.h>

static int insert_values_string(const struct _DBTable *tbl, size_t row, ValueSpecifier spec,struct _StringBuf *sql,size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	if( stringbuf_append(sql,"(") ) {
		return 1; }
	for(size_t col = 0; col < tbl->def->ncols; col++) {
		if(col && stringbuf_append(sql,",")) {
			return 1; }
		spec(&(tbl->def->cols[col]),tbl->rows.buf[row][col],sql,realserial);
		(*realserial)++;
	}
	if( stringbuf_append(sql,")") ) {
		return 1; }
	return 0;
}

int insert_values_row_string(const struct _DBTable *tbl,ValueSpecifier spec,struct _StringBuf *sql,size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	for(size_t row = 0; row < tbl->rows.nrows; row++) {
		if(row && stringbuf_append(sql,",")) {
			return 1;}
		insert_values_string(tbl,row,spec,sql,realserial);
	}
	return 0;
}

static int upsert_values_string(const struct _DBTable *tbl, size_t row, ValueSpecifier spec,struct _StringBuf *sql,size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	if( stringbuf_append(sql,"(") ) {
		return 1; }

	int printed = 0;
	for(size_t col = 0; col < tbl->def->ncols; col++) {
		if(!tbl->rows.isset[row][col]) {
			continue; }
		if(printed && stringbuf_append(sql,",")) {
			return 1; }
		spec(&(tbl->def->cols[col]),tbl->rows.buf[row][col],sql,realserial);
		(*realserial)++;
		printed++;
	}
	if( stringbuf_append(sql,")") ) {
		return 1; }
	return 0;
}

int upsert_values_row_string(const struct _DBTable *tbl,ValueSpecifier spec,struct _StringBuf *sql,size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	for(size_t row = 0; row < tbl->rows.nrows; row++) {
		if(row && stringbuf_append(sql,",")) {
			return 1;}
		upsert_values_string(tbl,row,spec,sql,realserial);
	}
	return 0;
}

int values_generic_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t *serial) {
	size_t bufsize = def->type == COL_TYPE_STRING ? def->size : 64;
	char *buf = alloca(bufsize);
	if(!buf) {
		return 1; }
	if(get_column_string(buf,bufsize,def,value)) {
		return 1; }
	if(stringbuf_append(sql,buf)) {
		return 1; }
	return 0;
}

int update_values_string(const struct _DBTable *tbl,ValueSpecifier spec,const char *delimiter,struct _StringBuf *sql,size_t *serial) {
	const struct _DBColumnDef *def = tbl->def->cols;
	const char *del = delimiter ? delimiter : "";
	size_t ncols = tbl->def->ncols;
	size_t printed = 0;
	for(size_t col = 0; col < ncols; col++) {
		if(!tbl->rows.isset[0][col]) {
			continue;}
		if(printed && stringbuf_append(sql,",")) {
			return 1;}
		if(stringbuf_appendf(sql,"%s%s%s = ",del,def[col].name,del)) {
			return 1;}
		if(spec(&(def[col]),tbl->rows.buf[0][col],sql,serial)) {
			return 1;}
		printed++;
		(*serial)++;
	}
	return 0;
}
