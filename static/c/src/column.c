
#include "column.h"

#include "logger.h"

#include <time.h>

const void* get_columnbuf_from_row(const struct _DBColumnDef *def,size_t colidx,const void *buf) {
	const char *rc = (char*)buf;
	for(size_t i = 0; i < colidx; i++) {
		size_t colsize = get_column_bufsize(def);
		if(!colsize) {
			return 0;}
		rc += colsize;
	}
	return rc;
}

size_t get_column_bufsize(const struct _DBColumnDef *col) {
	switch(col->type) {
	case COL_TYPE_STRING:
		return (col->size + 1);
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= 16) {
			return sizeof(short);
		} else if(col->size <= 32 || col->size == 0) {
			return sizeof(long);
		} else if (col->size <= 64) {
			return sizeof(long long);
		} else {
			LOGF_WARN("invalid int size: %lu",col->size);
			return 0;
		}
	case COL_TYPE_FLOAT:
		return sizeof(double);
	case COL_TYPE_DATETIME:
		return sizeof(struct tm);
	default:
		LOG_WARN("invalid datatype");
		return 0;
	}
}
