#include "table.h"

#include "string.h"
#include "column.h"

#include "logger.h"

int get_colidx_by_name(const char* colname,const struct _DBTableDef* def,size_t* idx) {
	if(!def||!idx||!colname) {
		return 1; }

	size_t i = 0;
	int found = 0;
	for(; i < def->ncols; i++) {
		if(strcmp(colname,def->cols[i].name) == 0) {
			found = 1;
			break;
		}
	}
	if(!found) {
		return 1; }

	*idx = i;
	return 0;
}

const void* get_colbuf_by_idx(size_t idx,const struct _DBTable* tbl) {
	if(!tbl||!tbl->valbuf) {
		return 0; }
	return &(tbl->valbuf[tbl->bytepos[idx]]);
}
