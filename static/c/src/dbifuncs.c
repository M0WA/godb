#ifndef _DISABLE_DBI

#include "dbifuncs.h"

#include "dbhandle_impl.h"
#include "hooks.h"
#include "dbihooks.h"
#include "logger.h"

int dbi_init_dblib() {
	return 0;
}

int dbi_exit_dblib() {
	return 0;
}

int dbi_init_dbh(struct _DBHandle *dbh) {
	dbi_initialize_r(NULL, &dbh->dbi.inst);
	dbh->dbi.result = 0;
	switch(dbh->config.dbi.type) {
	case DBI_TYPE_MYSQL:
		dbh->dbi.conn = dbi_conn_new_r("mysql", dbh->dbi.inst);
		break;
	case DBI_TYPE_POSTGRES:
		dbh->dbi.conn = dbi_conn_new_r("pgsql", dbh->dbi.inst);
		break;
	default:
		LOG_WARN("invalid dbi type");
		return 1;
	}
	if(!dbh->dbi.conn) {
		LOG_WARN("could not create dbi connection");
		return 1;
	}
	REGISTER_HOOKS(dbh,dbi);
	return 0;
}

#endif
