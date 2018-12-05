#include "values.h"

#include "column.h"
#include "helper.h"
#include "stringbuf.h"

#include <alloca.h>
#include <stdio.h>

static int insert_values_string(const struct _DBColumnDef *def, size_t ncols, ValueSpecifier spec, const void* const*const values,struct _StringBuf *sql,size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	if( stringbuf_append(sql,"(") ) {
		return 1; }
	size_t printedCols = 0;
	for(size_t i = 0; i < ncols; i++) {
		if(printedCols && stringbuf_append(sql,",")) {
			return 1; }
		spec(&(def[i]),values[i],sql,realserial);
		printedCols++;
		(*realserial)++;
	}
	if( stringbuf_append(sql,")") ) {
		return 1; }

	return 0;
}

int insert_values_row_string(const struct _DBColumnDef *def, size_t ncols, ValueSpecifier spec, const void * const* const* const values, size_t nrows,struct _StringBuf *sql, size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	for(size_t i = 0; i < nrows; i++) {
		if(i && stringbuf_append(sql,",")) {
			return 1;}
		insert_values_string(def,ncols,spec,values[i],sql,realserial);
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

int update_values_string(const struct _DBColumnDef *def, size_t ncols,ValueSpecifier spec, const void * const* values,struct _StringBuf *sql,size_t *serial) {
	size_t printed = 0;
	for(size_t col = 0; col < ncols; col++) {
		if(printed && stringbuf_append(sql,",")) {
			return 1;}
		if(stringbuf_append(sql,def[col].name)) {
			return 1;}
		if(stringbuf_append(sql,"=")) {
			return 1;}
		if(spec(&(def[col]),values[col],sql,serial)) {
			return 1;}
		printed++;
		(*serial)++;
	}
	return 0;
}
