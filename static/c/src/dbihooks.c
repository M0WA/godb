#ifndef _DISABLE_DBI

#include "dbihooks.h"
#include "dbhandle_impl.h"
#include "logger.h"

int dbi_connect_hook(struct _DBHandle *dbh) {
    dbi_conn_set_option(dbh->dbi.conn, "host", dbh->config.host /* dbh->config.port */);
    dbi_conn_set_option(dbh->dbi.conn, "username", dbh->config.user);
    dbi_conn_set_option(dbh->dbi.conn, "password", dbh->config.pass);
    dbi_conn_set_option(dbh->dbi.conn, "dbname", dbh->config.name);
    dbi_conn_set_option(dbh->dbi.conn, "encoding", "UTF-8");

	if(dbi_conn_connect(dbh->dbi.conn) < 0) {
		LOG_WARN("dbi_conn_connect() failed");
		return 1;}
	return 0;
}

int dbi_disconnect_hook(struct _DBHandle *dbh) {
	if(!dbh) {
		return 0; }
	dbi_conn_close(dbh->dbi.conn);
    dbi_shutdown_r(dbh->dbi.inst);
	return 0;
}

int dbi_insert_hook(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	return 1;
}

int dbi_update_hook(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	return 1;
}

int dbi_upsert_hook(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	return 1;
}

int dbi_delete_hook(struct _DBHandle *dbh,const struct _DeleteStmt *const s) {
	return 1;
}

int dbi_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult** res) {
	return 1;
}

#endif
