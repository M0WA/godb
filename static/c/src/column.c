
#include "column.h"

#include "logger.h"
#include "helper.h"

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
		if(col->size != 0 && col->size <= sizeof(short)) {
			return sizeof(short);
		} else if(col->size <= sizeof(long) || col->size == 0) {
			return sizeof(long);
		} else if (col->size <= sizeof(long long)) {
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

int get_column_string(char *colstr,size_t colen,const struct _DBColumnDef *col,const void *buf) {
	if(!buf) {
		snprintf(colstr,colen,"NULL");
		return 0; }

	switch(col->type) {
	case COL_TYPE_DATETIME:
		strftime(colstr, colen, "'%Y-%m-%d %H:%M:%S'", ((struct tm*)buf));
		break;
	case COL_TYPE_STRING:
		snprintf(colstr,colen,"'%s'",(char*)buf);
		break;
	case COL_TYPE_FLOAT:
		snprintf(colstr,colen,"%f",*((double*)buf));
		break;
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= 16) {
			snprintf(colstr,colen,"%d",*((short*)buf));
		} else if(col->size <= 32 || col->size == 0) {
			snprintf(colstr,colen,"%ld",*((long*)buf));
		} else if (col->size <= 64) {
			snprintf(colstr,colen,"%lld",*((long long*)buf));
		} else {
			LOGF_WARN("invalid int size: %lu",col->size);
			return 1;
		}
		break;
	default:
		LOG_WARN("invalid column type");
		return 1;
	}
	return 0;
}
