#ifndef _DISABLE_POSTGRES

#include "postgreshelper.h"

#include "where.h"

int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial) {
	return 0;
}

int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial) {
	return 0;
}

#endif
