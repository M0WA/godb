#ifndef _DISABLE_POSTGRES

#include "postgreshelper.h"

#include "where.h"
#include "column.h"
#include "helper.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alloca.h>

int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial) {
	size_t bufsize = def->type == COL_TYPE_STRING ? def->size : 64;
	char *buf = alloca(bufsize);
	if(!buf) {
		return 1; }
	if(get_column_string(buf,bufsize,def,value)) {
		return 1; }
	if(append_string(buf,sql)) {
		return 1; }
	return 0;
}

int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial) {
	size_t bufsize = def->type == COL_TYPE_STRING ? def->size : 64;
	char *buf = alloca(bufsize);
	if(!buf) {
		return 1; }
	if(get_column_string(buf,bufsize,def,value)) {
		return 1; }
	if(append_string(buf,sql)) {
		return 1; }
	return 0;
}

int postgres_time_to_tm(const char *val, struct tm *t) {
	memset(t, 0, sizeof(struct tm));
	if( !strptime(val, "%Y-%m-%d %H:%M:%S", t) ) {
		LOGF_WARN("invalid postgres date: %s",val);
		return 1;
	}
	return 0;
}

#endif
