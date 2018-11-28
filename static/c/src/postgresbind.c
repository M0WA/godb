#ifndef DISABLE_POSTGRES

#include "postgresbind.h"

int postgres_append_param(const DBColumnDef *def,size_t serial,const void *const val,PostgresParamWrapper *param) {
	switch(def->type) {
	case COL_TYPE_STRING:
		//TEXTOID			25
		break;
	case COL_TYPE_INT:
		//INT2OID			21
		//INT4OID			23
		//INT8OID			20
		break;
	case COL_TYPE_FLOAT:
		//FLOAT8OID
		break;
	case COL_TYPE_DATETIME:
		//TIMESTAMPOID	1114
		break;
	default:
		return 1;
	}

	return 1;
}

#endif
