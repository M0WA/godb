#ifndef _DISABLE_DBI

#include "dbihelper.h"
#include "column.h"
#include "helper.h"
#include "statements.h"
#include "mysqlhelper.h"
#include "postgreshelper.h"
#include "dbitypes.h"
#include "dbhandle_impl.h"

#include <stdlib.h>

int dbi_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial) {
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

int dbi_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial) {
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

int dbi_upsert_stmt_string(const struct _DBHandle *dbh,const UpsertStmt *const s, char** sql) {
	switch(dbh->config.dbi.type) {
	case DBI_TYPE_MYSQL:
		return mysql_upsert_stmt_string(s,sql);
	case DBI_TYPE_POSTGRES:
		return postgres_upsert_stmt_string(s,sql);
	default:
		return 1;
	}
	return 1;
}

#endif
