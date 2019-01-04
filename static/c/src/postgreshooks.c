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
#include "stringbuf.h"

#include <string.h>

static const char *postgres_delimiter = "\"";

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
	sprintf(port,"%d",dbh->cred.Port);
	dbh->postgres.conn = PQsetdbLogin(
			dbh->cred.Host,
			port,
			"", //options
			"", //tty
			dbh->cred.Name,
			dbh->cred.User,
			dbh->cred.Pass );
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
	LOG_DEBUG("connected to postgres-database");
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
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( insert_stmt_string(s,postgres_raw_value_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_INSERT_RAW_EXIT; }

	res = PQexec(dbh->postgres.conn, stringbuf_get(&stmtbuf));
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("PQexec(): %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_INSERT_RAW_EXIT; }

POSTGRES_INSERT_RAW_EXIT:
	if(res) {
		PQclear(res); }
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int postgres_insert_prepared(struct _DBHandle *dbh,struct _InsertStmt const*const s) {
	int rc = 0;
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( insert_stmt_string(s,postgres_values_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_INSERT_PREPARED_EXIT; }

	for(size_t row = 0; row < s->dbtbl->rows.nrows; row++) {
		if( postgres_values(s->dbtbl,row,&param) ) {
			rc = 1;
			goto POSTGRES_INSERT_PREPARED_EXIT; }
	}
	res = PQexecParams(dbh->postgres.conn, stringbuf_get(&stmtbuf), param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("PQexecParams(): %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_INSERT_PREPARED_EXIT; }

POSTGRES_INSERT_PREPARED_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(res) {
		PQclear(res); }
	return rc;
}

int postgres_insert_hook(struct _DBHandle *dbh,struct _InsertStmt const*const s) {
	if(dbh->config.Postgres.Preparedstatements) {
		return postgres_insert_prepared(dbh,s);
	} else {
		return postgres_insert_raw(dbh,s);
	}
	return 1;
}

static int postgres_delete_raw(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	int rc = 0;
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( delete_stmt_string(s,where_generic_value_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_DELETE_RAW_EXIT; }

	res = PQexec(dbh->postgres.conn, stringbuf_get(&stmtbuf));
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("delete failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_DELETE_RAW_EXIT; }

POSTGRES_DELETE_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(res) {
		PQclear(res); }
	return rc;
}

static int postgres_delete_prepared(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	int rc = 0;
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( delete_stmt_string(s,postgres_where_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_DELETE_PREPARED_EXIT; }

	if( postgres_where(&s->where,&param) ) {
		rc = 1;
		goto POSTGRES_DELETE_PREPARED_EXIT; }

	res = PQexecParams(dbh->postgres.conn, stringbuf_get(&stmtbuf), param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("PQexecParams(): %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_DELETE_PREPARED_EXIT; }

POSTGRES_DELETE_PREPARED_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(res) {
		PQclear(res); }
	return rc;
}

int postgres_delete_hook(struct _DBHandle *dbh,struct _DeleteStmt const*const s) {
	if(dbh->config.Postgres.Preparedstatements) {
		return postgres_delete_prepared(dbh,s);
	} else {
		return postgres_delete_raw(dbh,s);
	}
	return 1;
}

static int postgres_select_raw(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( select_stmt_string(s,where_generic_value_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_SELECT_RAW_EXIT; }

	dbh->postgres.res = PQexec(dbh->postgres.conn, stringbuf_get(&stmtbuf));
	if (PQresultStatus(dbh->postgres.res) != PGRES_TUPLES_OK) {
		LOGF_WARN("select failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_RAW_EXIT; }
	dbh->postgres.resrow = 0;

	if( create_selectresult(s->def,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_RAW_EXIT;}

POSTGRES_SELECT_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(rc) {
		dbh->postgres.resrow = 0;
		dbh->postgres.res = 0; }
	return rc;
}

static int postgres_select_prepared(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( select_stmt_string(s,postgres_where_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_SELECT_PREPARED_EXIT; }

	if( postgres_where(&s->where,&param) ) {
		rc = 1;
		goto POSTGRES_SELECT_PREPARED_EXIT; }

	dbh->postgres.res = PQexecParams(dbh->postgres.conn, stringbuf_get(&stmtbuf), param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(dbh->postgres.res) != PGRES_TUPLES_OK) {
		LOGF_WARN("select failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_PREPARED_EXIT; }
	dbh->postgres.resrow = 0;

	if( create_selectresult(s->def,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		PQclear(dbh->postgres.res);
		goto POSTGRES_SELECT_PREPARED_EXIT;}

POSTGRES_SELECT_PREPARED_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

int postgres_select_hook(struct _DBHandle *dbh,struct _SelectStmt const*const s,struct _SelectResult *res) {
	if(dbh->config.Postgres.Preparedstatements) {
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
		return 0;
	}
	size_t ncols = PQnfields(dbh->postgres.res);
	for (size_t residx = 0; residx < ncols; residx++) {
		if(PQgetisnull(dbh->postgres.res, dbh->postgres.resrow, residx)) {
			setnull_dbtable_columnbuf(&res->tbl, 0, residx);
		} else {
			char *val = PQgetvalue(dbh->postgres.res, dbh->postgres.resrow, residx);
			if(!val) {
				return -1; }
			void *colbuf = set_dbtable_columnbuf(&res->tbl, 0, residx);
			const DBColumnDef *col = &(res->tbl.def->cols[residx]);
			if( set_columnbuf_by_string(col,postgres_time_to_tm,colbuf,val) ) {
				return -1;
			}
		}
	}
	dbh->postgres.resrow++;
	return 1;
}

static int postgres_update_raw(struct _DBHandle *dbh,struct _UpdateStmt const*const s) {
	int rc = 0;
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( update_stmt_string(s,values_generic_value_specifier,where_generic_value_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_UPDATE_RAW_EXIT; }

	res = PQexec(dbh->postgres.conn, stringbuf_get(&stmtbuf));
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("update failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_UPDATE_RAW_EXIT; }

POSTGRES_UPDATE_RAW_EXIT:
	if(res) {
		PQclear(res); }
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int postgres_update_prepared(struct _DBHandle *dbh,struct _UpdateStmt const*const s) {
	int rc = 0;
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	if( postgres_update_values(s->dbtbl,0,&param) ) {
		rc = 1;
		goto POSTGRES_UPDATE_PREPARED_EXIT; }

	if( postgres_where(&s->where,&param) ) {
		rc = 1;
		goto POSTGRES_UPDATE_PREPARED_EXIT; }

	if( update_stmt_string(s,postgres_values_specifier,postgres_where_specifier,postgres_delimiter,&stmtbuf) ) {
		rc = 1;
		goto POSTGRES_UPDATE_PREPARED_EXIT; }

	res = PQexecParams(dbh->postgres.conn, stringbuf_get(&stmtbuf), param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("update failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_UPDATE_PREPARED_EXIT; }

POSTGRES_UPDATE_PREPARED_EXIT:
	if(res) {
		PQclear(res); }
	stringbuf_destroy(&stmtbuf);
	return rc;
}

int postgres_update_hook(struct _DBHandle *dbh,struct _UpdateStmt const*const s) {
	if(dbh->config.Postgres.Preparedstatements) {
		return postgres_update_prepared(dbh,s);
	} else {
		return postgres_update_raw(dbh,s);
	}
	return 1;
}

static int postgres_upsert_raw(struct _DBHandle *dbh,struct _UpsertStmt const*const s) {
	int rc = 0;
	PGresult *res = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if(postgres_upsert_stmt_string(s, values_generic_value_specifier,postgres_delimiter, &stmtbuf)) {
		rc = 1;
		goto POSTGRES_UPSERT_RAW_EXIT; }

	res = PQexec(dbh->postgres.conn, stringbuf_get(&stmtbuf));
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("update failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_UPSERT_RAW_EXIT; }

POSTGRES_UPSERT_RAW_EXIT:
	if(res) {
		PQclear(res); }
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int postgres_upsert_prepared(struct _DBHandle *dbh,struct _UpsertStmt const*const s) {
	/*
	int rc = 0;
	PGresult *res = 0;

	PostgresParamWrapper param;
	memset(&param,0,sizeof(PostgresParamWrapper));

	//TODO: fill param with real values

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if(postgres_upsert_stmt_string(s, postgres_values_specifier, &stmtbuf)) {
		rc = 1;
		goto POSTGRES_UPSERT_PREPARED_EXIT; }

	res = PQexecParams(dbh->postgres.conn, stmtbuf, param.nparam, param.types,param.values,param.lengths,param.formats,0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		LOGF_WARN("upsert failed: %s",PQerrorMessage(dbh->postgres.conn));
		rc = 1;
		PQclear(res);
		goto POSTGRES_UPSERT_PREPARED_EXIT; }

POSTGRES_UPSERT_PREPARED_EXIT:
	if(res) {
		PQclear(res); }
	stringbuf_destroy(&stmtbuf);
	return rc;
	*/
	LOG_WARN("postgres prepared upsert not implemented, falling back to raw...");
	return postgres_upsert_raw(dbh,s);
}

int postgres_upsert_hook(struct _DBHandle *dbh,struct _UpsertStmt const*const s) {
	if(dbh->config.Postgres.Preparedstatements) {
		return postgres_upsert_prepared(dbh,s);
	} else {
		return postgres_upsert_raw(dbh,s);
	}
	return 1;
}

#endif
