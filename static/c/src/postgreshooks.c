#ifndef _DISABLE_POSTGRES

#include "postgreshooks.h"

#include "dbhandle_impl.h"
#include "logger.h"

int postgres_connect_hook(struct _DBHandle *dbh) {
	if(!dbh) {
		LOG_WARN("handle is null");
		return 1;
	}
	if(dbh->postgres.conn) {
		LOG_WARN("postgres handle is already initalized");
		return 1;
	}

	char port[6] = {0};
	sprintf(port,"%d",dbh->config.port);
	dbh->postgres.conn = PQsetdbLogin(
			dbh->config.host,
			port,
			"", //options
			"", //tty
			dbh->config.name,
			dbh->config.user,
			dbh->config.pass );
	if(!dbh->postgres.conn) {
		LOG_WARN("postgres handle failed to initalize");
		return 1;
	}
	if (PQstatus(dbh->postgres.conn) != CONNECTION_OK) {
		LOGF_WARN("Connection to database failed: %s",PQerrorMessage(dbh->postgres.conn));
		if(dbh->postgres.conn) {
			postgres_disconnect_hook(dbh); }
		return 1;
	}
	return 0;
}

int postgres_disconnect_hook(struct _DBHandle *dbh) {
	if(!dbh) {
		LOG_WARN("handle is null");
		return 1;
	}
	if(!dbh->postgres.conn) {
		LOG_WARN("postgres handle is not initalized");
		return 1;
	}
	PQfinish(dbh->postgres.conn);
	dbh->postgres.conn = 0;

	LOG_DEBUG("disconnected from postgres-database");
	return 0;
}

int postgres_insert_hook(struct _DBHandle *dbh,struct _InsertStmt const*const s) {
	return 1;
}

int postgres_update_hook(struct _DBHandle *dbh,struct _UpdateStmt const*const s) {
	return 1;
}

int postgres_upsert_hook(struct _DBHandle *dbh,struct _UpsertStmt const*const s) {
	return 1;
}

int postgres_delete_hook(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	return 1;
}

int postgres_select_hook(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	return 1;
}

int postgres_fetch_hook(struct _DBHandle *dbh,struct _SelectResult *res) {
	return 1;
}

#endif
