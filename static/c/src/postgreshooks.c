#ifndef _DISABLE_POSTGRES

#include "postgreshooks.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "statements.h"
#include "helper.h"
#include "postgreshelper.h"

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
	sprintf(port,"%d",dbh->cred.port);
	dbh->postgres.conn = PQsetdbLogin(
			dbh->cred.host,
			port,
			"", //options
			"", //tty
			dbh->cred.name,
			dbh->cred.user,
			dbh->cred.pass );
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
	int rc = 0;
	char *stmtbuf = 0;

	if( insert_stmt_string(s,postgres_values_specifier,&stmtbuf, 1) ) {
		rc = 1;
		goto POSTGRES_INSERT_EXIT; }

	//TODO:

POSTGRES_INSERT_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	return rc;
}

int postgres_delete_hook(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	char *stmtbuf = 0;
	int rc = 0;

	if( delete_stmt_string(s,postgres_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_DELETE_EXIT; }

	//TODO:

POSTGRES_DELETE_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	return rc;
}

int postgres_select_hook(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	char *stmtbuf = 0;
	int rc = 0;

	if( select_stmt_string(s,postgres_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_SELECT_EXIT; }

	//TODO:

POSTGRES_SELECT_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	return rc;
}

int postgres_fetch_hook(struct _DBHandle *dbh,struct _SelectResult *res) {
	return 0;
}

int postgres_update_hook(struct _DBHandle *dbh,struct _UpdateStmt const*const s) {
	return 1;
}

int postgres_upsert_hook(struct _DBHandle *dbh,struct _UpsertStmt const*const s) {
	return 1;
}

#endif
