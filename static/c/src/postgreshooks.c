#ifndef _DISABLE_POSTGRES

#include "postgreshooks.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "statements.h"
#include "helper.h"
#include "selectresult.h"
#include "column.h"
#include "postgreshelper.h"
#include "postgresbind.h"

#include <string.h>

int postgres_initlib_hook() {
	if(PQisthreadsafe() == 0) {
		LOG_FATAL(1,"please use a thread-safe version of pq library"); }
	return 0;
}

int postgres_exitlib_hook() {
	return 0;
}

int postgres_create_hook(struct _DBHandle *dbh) {
	if(dbh->postgres.conn) {
		LOG_WARN("postgres handle is already initalized"); }
	dbh->postgres.res = 0;
	return 0;
}

int postgres_destroy_hook(struct _DBHandle *dbh) {
	return 0;
}

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

static int postgres_insert_raw(struct _DBHandle *dbh,struct _InsertStmt const*const s) {
	int rc = 0;
	char *stmtbuf = 0;
	PGresult *res = 0;

	if( insert_stmt_string(s,values_generic_value_specifier,&stmtbuf, 1) ) {
		rc = 1;
		goto POSTGRES_INSERT_RAW_EXIT; }

	res = PQexec(dbh->postgres.conn, stmtbuf);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("PQexec(): %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_INSERT_RAW_EXIT; }

POSTGRES_INSERT_RAW_EXIT:
	if(res) {
		PQclear(res); }
	if(stmtbuf) {
		free(stmtbuf); }
	return rc;
}

static int postgres_insert_prepared(struct _DBHandle *dbh,struct _InsertStmt const*const s) {
	int rc = 0;
	char *stmtbuf = 0;
	PGresult *res = 0;

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( insert_stmt_string(s,postgres_values_specifier,&stmtbuf,1) ) {
		rc = 1;
		goto POSTGRES_INSERT_PREPARED_EXIT; }

	for(size_t row = 0; row < s->nrows; row++) {
		if( postgres_values(s->defs,s->ncols,s->valbuf[row],&param) ) {
			rc = 1;
			goto POSTGRES_INSERT_PREPARED_EXIT; }
	}
	res = PQexecParams(dbh->postgres.conn, stmtbuf, param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("PQexecParams(): %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_INSERT_PREPARED_EXIT; }

POSTGRES_INSERT_PREPARED_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	if(res) {
		PQclear(res); }
	return rc;
}

int postgres_insert_hook(struct _DBHandle *dbh,struct _InsertStmt const*const s) {
	if(dbh->config.postgres.preparedstatements) {
		return postgres_insert_prepared(dbh,s);
	} else {
		return postgres_insert_raw(dbh,s);
	}
	return 1;
}

static int postgres_delete_raw(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	char *stmtbuf = 0;
	int rc = 0;
	PGresult *res = 0;

	if( delete_stmt_string(s,where_generic_value_specifier,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_DELETE_RAW_EXIT; }

	res = PQexec(dbh->postgres.conn, stmtbuf);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("delete failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_DELETE_RAW_EXIT; }

POSTGRES_DELETE_RAW_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	if(res) {
		PQclear(res); }
	return rc;
}

static int postgres_delete_prepared(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	int rc = 0;
	char *stmtbuf = 0;
	PGresult *res = 0;

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( delete_stmt_string(s,postgres_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_DELETE_PREPARED_EXIT; }

	if( postgres_where(&s->where,&param) ) {
		rc = 1;
		goto POSTGRES_DELETE_PREPARED_EXIT; }

	res = PQexecParams(dbh->postgres.conn, stmtbuf, param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("PQexecParams(): %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_DELETE_PREPARED_EXIT; }

POSTGRES_DELETE_PREPARED_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	if(res) {
		PQclear(res); }
	return rc;
}

int postgres_delete_hook(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	if(dbh->config.postgres.preparedstatements) {
		return postgres_delete_prepared(dbh,s);
	} else {
		return postgres_delete_raw(dbh,s);
	}
	return 1;
}

static int postgres_select_raw(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	char *stmtbuf = 0;
	int rc = 0;

	if( select_stmt_string(s,where_generic_value_specifier,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_SELECT_RAW_EXIT; }

	dbh->postgres.res = PQexec(dbh->postgres.conn, stmtbuf);
	if (PQresultStatus(dbh->postgres.res) != PGRES_TUPLES_OK) {
		LOGF_WARN("select failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_RAW_EXIT; }
	dbh->postgres.resrow = 0;

	if( create_selectresult(s->defs,s->ncols,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_RAW_EXIT;}

POSTGRES_SELECT_RAW_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	if(rc) {
		dbh->postgres.resrow = 0;
		dbh->postgres.res = 0; }
	return rc;
}

static int postgres_select_prepared(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	int rc = 0;
	char *stmtbuf = 0;

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( select_stmt_string(s,postgres_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_SELECT_PREPARED_EXIT; }

	if( postgres_where(&s->where,&param) ) {
		rc = 1;
		goto POSTGRES_SELECT_PREPARED_EXIT; }

	dbh->postgres.res = PQexecParams(dbh->postgres.conn, stmtbuf, param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(dbh->postgres.res) != PGRES_TUPLES_OK) {
		LOGF_WARN("select failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_PREPARED_EXIT; }
	dbh->postgres.resrow = 0;

	if( create_selectresult(s->defs,s->ncols,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_PREPARED_EXIT;}

POSTGRES_SELECT_PREPARED_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	return rc;
}

int postgres_select_hook(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	if(dbh->config.postgres.preparedstatements) {
		return postgres_select_prepared(dbh,s,res);
	} else {
		return postgres_select_raw(dbh,s,res);
	}
	return 1;
}

int postgres_fetch_hook(struct _DBHandle *dbh,struct _SelectResult *res) {
	if( dbh->postgres.resrow == PQntuples(dbh->postgres.res) ) {
		PQclear(dbh->postgres.res);
		dbh->postgres.res = 0;
		dbh->postgres.resrow = 0;
		destroy_selectresult(res);
		return 0;
	}
	size_t ncols = PQnfields(dbh->postgres.res);
	for (size_t residx = 0; residx < ncols; residx++) {
		if(PQgetisnull(dbh->postgres.res, dbh->postgres.resrow, residx)) {
			res->row[residx] = 0;
		} else {
			char *val = PQgetvalue(dbh->postgres.res, dbh->postgres.resrow, residx);
			if(!val) {
				return 0; }
			const DBColumnDef *col = &(res->cols[residx]);
			if( set_columnbuf_by_string(col,postgres_time_to_tm,res->row[residx],val) ) {
				return -1;
			}
		}
	}
	dbh->postgres.resrow++;
	return 1;
}

int postgres_update_hook(struct _DBHandle *dbh,struct _UpdateStmt const*const s) {
	return 1;
}

int postgres_upsert_hook(struct _DBHandle *dbh,struct _UpsertStmt const*const s) {
	//int postgres_upsert_stmt_string(const UpsertStmt *const s, char** sql)
	return 1;
}

#endif
