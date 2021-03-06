#ifndef _DISABLE_DBI

#include "dbihelper.h"

#include "mysqlhelper.h"
#include "postgreshelper.h"
#include "dbhandle_impl.h"
#include "values.h"

int dbi_upsert_stmt_string(const struct _DBHandle *dbh,const struct _UpsertStmt *const s, const char *delimiter, struct _StringBuf *sql) {
	switch(dbh->config.Dbi.Type) {
	case DBI_TYPE_MYSQL:
		return mysql_upsert_stmt_string(s,values_generic_value_specifier,delimiter,sql);
	case DBI_TYPE_POSTGRES:
		return postgres_upsert_stmt_string(s,values_generic_value_specifier,delimiter,sql);
	default:
		return 1;
	}
	return 1;
}

#endif
