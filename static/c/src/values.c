#include "values.h"

#include "column.h"
#include "helper.h"

static int insert_values_string(const struct _DBColumnDef *def, size_t ncols, ValueSpecifier spec, const void* const*const values,char** sql,size_t *serial,int skip_autoincrement) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	if( append_string("(",sql) ) {
		return 1; }
	size_t printedCols = 0;
	for(size_t i = 0; i < ncols; i++) {
		if(def[i].autoincrement && skip_autoincrement) {
			continue; }
		if(printedCols && append_string(",",sql)) {
			return 1; }
		spec(&(def[i]),values[i],sql,realserial);
		printedCols++;
		(*realserial)++;
	}
	if( append_string(")",sql) ) {
		return 1; }

	return 0;
}

int insert_values_row_string(const struct _DBColumnDef *def, size_t ncols, ValueSpecifier spec, const void * const* const* const values, size_t nrows, char** sql, size_t *serial,int skip_autoincrement) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;

	for(size_t i = 0; i < nrows; i++) {
		if(i && append_string(",",sql)) {
			return 1;}
		insert_values_string(def,ncols,spec,values[i],sql,realserial,skip_autoincrement);
	}
	return 0;
}

int update_values_string(const struct _DBColumnDef *def, size_t ncols,ValueSpecifier spec, const void * const* values,char** sql,size_t *serial,int skip_autoincrement) {
	return 0;
}
