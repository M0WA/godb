#ifndef DISABLE_POSTGRES

#include <stdlib.h>
#include <string.h>

#include "postgresbind.h"
#include "column.h"
#include "logger.h"

int postgres_param_append(const struct _DBColumnDef *col,size_t serial,const void *const val,PostgresParamWrapper *param) {
	switch(col->type) {
	case COL_TYPE_STRING:
		param->types[param->nparam] = 25; //TEXTOID
		param->formats[param->nparam] = 0;
		param->lengths[param->nparam] = strlen((const char*)val);
		param->values[param->nparam] = val;
		break;
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= sizeof(short)) {
			param->types[param->nparam] = 21; //INT2OID
		} else if(col->size <= sizeof(long) || col->size == 0) {
			param->types[param->nparam] = 23; //INT4OID
		} else if (col->size <= sizeof(long long)) {
			param->types[param->nparam] = 20; //INT8OID
		} else {
			LOGF_WARN("invalid int size: %lu",col->size);
			return 0;
		}
		param->formats[param->nparam] = 0;
		break;
	case COL_TYPE_FLOAT:
		param->types[param->nparam] = 701; //FLOAT8OID
		param->formats[param->nparam] = 0;
		break;
	case COL_TYPE_DATETIME:
		param->types[param->nparam] = 1114; //TIMESTAMPOID;
		param->formats[param->nparam] = 0;
		break;
	default:
		return 1;
	}

	param->nparam++;
	return 1;
}

#endif
