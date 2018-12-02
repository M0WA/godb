#ifndef _DISABLE_MYSQL

#include "mysqlhooks.h"

#include <string.h>
#include <time.h>

#include "dbhandle_impl.h"
#include "statements.h"
#include "column.h"
#include "logger.h"
#include "helper.h"
#include "mysqlhelper.h"
#include "mysqlbind.h"
#include "selectresult.h"
#include "values.h"
#include "stringbuf.h"

int mysql_initlib_hook() {
	if (!mysql_thread_safe()) {
		LOG_FATAL(1,"please use a thread-safe version of mysqlclient library");	}
	return mysql_library_init(0, NULL, NULL);
}

int mysql_exitlib_hook() {
	mysql_library_end();
	return 0;
}

int mysql_create_hook(struct _DBHandle* dbh) {
	if(dbh->mysql.conn) {
		LOG_WARN("mysql handle is already initalized");	}
	dbh->mysql.stmt = 0;
	return 0;
}

int mysql_destroy_hook(struct _DBHandle* dbh) {
	return 0;
}

int mysql_connect_hook(struct _DBHandle *dbh) {
	if(!dbh) {
		LOG_WARN("handle is null");
		return 1;
	}
	if(dbh->mysql.conn) {
		LOG_WARN("mysql handle is already initalized");
		return 1;
	}
	if(! (dbh->mysql.conn = mysql_init(dbh->mysql.conn)) ) {
		LOG_WARN("mysql handle failed to initalize");
		return 1;
	}

	my_bool one = 1;
	if(dbh->config.mysql.autoreconnect) {
		mysql_options(dbh->mysql.conn,MYSQL_OPT_RECONNECT,(const char*)&one); }
	if(dbh->config.mysql.compression) {
		mysql_options(dbh->mysql.conn,MYSQL_OPT_COMPRESS, (const char*)&one); }

	dbh->mysql.conn = mysql_real_connect(
			dbh->mysql.conn,
			dbh->cred.host,
			dbh->cred.user,
			dbh->cred.pass,
			dbh->cred.name,
			dbh->cred.port,
			NULL,
			CLIENT_MULTI_STATEMENTS |
			(dbh->config.mysql.compression ? CLIENT_COMPRESS : 0) |
			CLIENT_IGNORE_SIGPIPE );

	if(!dbh->mysql.conn) {
		LOGF_WARN("mysql connect: %s",mysql_error(dbh->mysql.conn));
		return 1;
	}

	mysql_autocommit(dbh->mysql.conn,1);
	LOG_DEBUG("connected to mysql-database");
	return 0;
}

int mysql_disconnect_hook(struct _DBHandle *dbh) {
	if(!dbh) {
		LOG_WARN("handle is null");
		return 1;
	}
	if(!dbh->mysql.conn) {
		LOG_WARN("mysql handle is not initalized");
		return 1;
	}
	mysql_close(dbh->mysql.conn);
	dbh->mysql.conn = 0;

	LOG_DEBUG("disconnected from mysql-database");
	return 0;
}

static int mysql_insert_raw(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	int rc = 0;
	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( insert_stmt_string(s,values_generic_value_specifier,&stmtbuf,0) ) {
		rc = 1;
		goto MYSQL_INSERT_RAW_EXIT; }

	if( mysql_real_query(dbh->mysql.conn, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_WARN("mysql_real_query(): %s", mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_INSERT_RAW_EXIT;	}

MYSQL_INSERT_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int mysql_insert_prepared(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	int rc = 0;
	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	for(size_t row = 0; row < s->nrows; row++) {
		if( mysql_values(s->defs,s->ncols,s->valbuf[row],&bind) ) {
			rc = 1;
			goto MYSQL_INSERT_PREPARED_EXIT; }
	}

	if( insert_stmt_string(s,mysql_values_specifier,&stmtbuf,0) ) {
		rc = 1;
		goto MYSQL_INSERT_PREPARED_EXIT; }

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_INSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_prepare(dbh->mysql.stmt, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_INSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_param_count(dbh->mysql.stmt) != (s->ncols * s->nrows) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOG_WARN("mysql_stmt_param_count: invalid column count in prepared statement");
		rc = 1;
		goto MYSQL_INSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_INSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_INSERT_PREPARED_EXIT;
	}

MYSQL_INSERT_PREPARED_EXIT:
	if(dbh->mysql.stmt) {
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;}
	stringbuf_destroy(&stmtbuf);
	return rc;
}

int mysql_insert_hook(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	if(dbh->config.mysql.preparedstatements) {
		return mysql_insert_prepared(dbh,s);
	} else {
		return mysql_insert_raw(dbh,s);
	}
	return 1;
}

static int mysql_delete_raw(struct _DBHandle *dbh,const struct _DeleteStmt *const s) {
	int rc = 0;
	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( delete_stmt_string(s,where_generic_value_specifier,&stmtbuf) ) {
		rc = 1;
		goto MYSQL_DELETE_RAW_EXIT; }

	if( mysql_real_query(dbh->mysql.conn, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_WARN("mysql_real_query(): %s", mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_DELETE_RAW_EXIT;	}

MYSQL_DELETE_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int mysql_delete_prepared(struct _DBHandle *dbh,const struct _DeleteStmt *const s) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	if( mysql_where(&s->where,&bind) ) {
		rc = 1;
		goto MYSQL_DELETE_PREPARED_EXIT; }

	if( delete_stmt_string(s,mysql_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto MYSQL_DELETE_PREPARED_EXIT; }

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_DELETE_PREPARED_EXIT; }
	if( mysql_stmt_prepare(dbh->mysql.stmt, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_DELETE_PREPARED_EXIT; }
	if( bind.bind_idx && mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_DELETE_PREPARED_EXIT; }
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_DELETE_PREPARED_EXIT; }

MYSQL_DELETE_PREPARED_EXIT:
	if(dbh->mysql.stmt) {
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;}
	stringbuf_destroy(&stmtbuf);
	return rc;
}

int mysql_delete_hook(struct _DBHandle *dbh,const struct _DeleteStmt *const s) {
	if(dbh->config.mysql.preparedstatements) {
		return mysql_delete_prepared(dbh,s);
	} else {
		return mysql_delete_raw(dbh,s);
	}
	return 1;
}

static int mysql_select_raw(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult *res) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( select_stmt_string(s,where_generic_value_specifier,&stmtbuf) ) {
		rc = 1;
		goto MYSQL_SELECT_RAW_EXIT; }

	if( mysql_real_query(dbh->mysql.conn, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_WARN("mysql_real_query(): %s", mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_SELECT_RAW_EXIT;	}

	dbh->mysql.res = mysql_use_result(dbh->mysql.conn);
	if(!dbh->mysql.res && mysql_field_count(dbh->mysql.conn)>0) {
		LOGF_WARN("mysql_use_result(): %s", mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_SELECT_RAW_EXIT;	}

	if( create_selectresult(s->defs,s->ncols,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		goto MYSQL_SELECT_RAW_EXIT;}

MYSQL_SELECT_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int mysql_select_prepared(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult *res) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));
	if( mysql_where(&s->where,&bind) ) {
		rc = 1;
		goto MYSQL_SELECT_PREPARED_EXIT; }

	if( select_stmt_string(s,mysql_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto MYSQL_SELECT_PREPARED_EXIT; }

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_SELECT_PREPARED_EXIT; }
	if( mysql_stmt_prepare(dbh->mysql.stmt, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_SELECT_PREPARED_EXIT; }
	if( bind.bind_idx && mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_SELECT_PREPARED_EXIT; }
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_SELECT_PREPARED_EXIT; }

	if( create_selectresult(s->defs,s->ncols,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		goto MYSQL_SELECT_PREPARED_EXIT;}

MYSQL_SELECT_PREPARED_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

int mysql_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult *res) {
	if(dbh->config.mysql.preparedstatements) {
		return mysql_select_prepared(dbh,s,res);
	} else {
		return mysql_select_raw(dbh,s,res);
	}
	return 1;
}

static int mysql_fetch_raw(struct _DBHandle *dbh,struct _SelectResult *res) {
	MYSQL_ROW row;

	if(!dbh->mysql.res) {
		return 0; }

	unsigned int num_fields = mysql_num_fields(dbh->mysql.res);

	if(!(row = mysql_fetch_row(dbh->mysql.res))) {
		mysql_free_result(dbh->mysql.res);
		dbh->mysql.res = 0;
		return 0;
	}
	for(size_t col = 0; col < num_fields; col++) {
		if(!row[col]) {
			res->row[col] = 0;
		} else if (	set_columnbuf_by_string(&(res->cols[col]),mysql_string_to_tm,res->row[col],row[col]) ) {
			return -1;
		}
	}
	return 1;
}

static int mysql_fetch_prepared(struct _DBHandle *dbh,struct _SelectResult *res) {
	MYSQL_BIND bind[MAX_BIND_COLS];
	my_bool is_null[MAX_BIND_COLS];
	unsigned long length[MAX_BIND_COLS];
	MYSQL_TIME times[MAX_BIND_COLS];

	if(!res || !res->ncols || !dbh || !dbh->mysql.stmt || res->ncols > MAX_BIND_COLS) {
		destroy_selectresult(res);
		return -1;
	}

	memset(&bind,0,sizeof(MYSQL_BIND) * MAX_BIND_COLS);
	memset(&is_null,0,sizeof(my_bool) * MAX_BIND_COLS);
	memset(&length,0,sizeof(unsigned long) * MAX_BIND_COLS);

	for(size_t i = 0; i < res->ncols; i++) {
		if( mysql_datatype(&(res->cols[i]),&(bind[i].buffer_type)) ) {
			destroy_selectresult(res);
			return -1; }
		if(res->cols[i].type == COL_TYPE_DATETIME) {
			bind[i].buffer_length = sizeof(MYSQL_TIME);
			bind[i].buffer = &times[i];
		} else {
			bind[i].buffer_length = get_column_bufsize(&(res->cols[i]));
			bind[i].buffer = res->row[i];
		}
		bind[i].length = &(length[i]);
		bind[i].is_null = &(is_null[i]);
		memset(bind[i].buffer,0,bind[i].buffer_length);
	}

	if (mysql_stmt_bind_result(dbh->mysql.stmt, &(bind[0]))) {
		LOGF_WARN("mysql_stmt_bind_result: %s", mysql_stmt_error(dbh->mysql.stmt));
		destroy_selectresult(res);
		return -1;
	}
	if ( mysql_stmt_fetch(dbh->mysql.stmt) ) {
		destroy_selectresult(res);
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;
		return 0;
	}
	for(size_t i = 0; i < res->ncols; i++) {
		if(res->cols[i].type == COL_TYPE_DATETIME) {
			mysql_tm(&times[i], res->row[i]);
		}
	}
	return 1;
}

int mysql_fetch_hook(struct _DBHandle *dbh,struct _SelectResult *res) {
	if(dbh->config.mysql.preparedstatements) {
		return mysql_fetch_prepared(dbh,res);
	} else {
		return mysql_fetch_raw(dbh,res);
	}
	return -1;
}

static int mysql_update_raw(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( update_stmt_string(s,values_generic_value_specifier,where_generic_value_specifier,&stmtbuf,1) ) {
		rc = 1;
		goto MYSQL_UPDATE_RAW_EXIT; }

	if( mysql_real_query(dbh->mysql.conn, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_WARN("mysql_real_query(): %s", mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_UPDATE_RAW_EXIT;	}

MYSQL_UPDATE_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int mysql_update_prepared(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	int rc = 0;
	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( update_stmt_string(s,mysql_values_specifier,mysql_where_specifier,&stmtbuf,1) ) {
		rc = 1;
		goto MYSQL_UPDATE_PREPARED_EXIT; }

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	if( mysql_values(s->defs,s->ncols,s->valbuf,&bind) ) {
		rc = 1;
		goto MYSQL_UPDATE_PREPARED_EXIT; }

	if( mysql_where(&s->where,&bind) ) {
		rc = 1;
		goto MYSQL_UPDATE_PREPARED_EXIT; }

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_UPDATE_PREPARED_EXIT; }
	if( mysql_stmt_prepare(dbh->mysql.stmt, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_UPDATE_PREPARED_EXIT; }
	if( bind.bind_idx && mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_UPDATE_PREPARED_EXIT; }
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_UPDATE_PREPARED_EXIT; }

MYSQL_UPDATE_PREPARED_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(dbh->mysql.stmt) {
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;}
	return rc;
}

int mysql_update_hook(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	if(dbh->config.mysql.preparedstatements) {
		return mysql_update_prepared(dbh,s);
	} else {
		return mysql_update_raw(dbh,s);
	}
	return 1;
}

static int mysql_upsert_raw(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( mysql_upsert_stmt_string(s, values_generic_value_specifier, &stmtbuf) ) {
		rc = 1;
		goto MYSQL_UPSERT_RAW_EXIT;
	}

	if( mysql_real_query(dbh->mysql.conn, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_WARN("mysql_real_query(): %s", mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_UPSERT_RAW_EXIT;	}

MYSQL_UPSERT_RAW_EXIT:
	stringbuf_destroy(&stmtbuf);
	return rc;
}

static int mysql_upsert_prepared(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	for(size_t row = 0; row < s->nrows; row++) {
		if( mysql_values(s->defs,s->ncols,s->valbuf[row],&bind) ) {
			rc = 1;
			goto MYSQL_UPSERT_PREPARED_EXIT; }
	}

	if( mysql_upsert_stmt_string(s, mysql_values_specifier, &stmtbuf) ) {
		rc = 1;
		goto MYSQL_UPSERT_PREPARED_EXIT;
	}

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_UPSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_prepare(dbh->mysql.stmt, stringbuf_get(&stmtbuf), stringbuf_strlen(&stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_UPSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_param_count(dbh->mysql.stmt) != (s->ncols * s->nrows) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOG_WARN("mysql_stmt_param_count: invalid column count in prepared statement");
		rc = 1;
		goto MYSQL_UPSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_UPSERT_PREPARED_EXIT;
	}
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_UPSERT_PREPARED_EXIT;
	}

MYSQL_UPSERT_PREPARED_EXIT:
	if(dbh->mysql.stmt) {
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;}
	stringbuf_destroy(&stmtbuf);
	return rc;
}

int mysql_upsert_hook(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	if(dbh->config.mysql.preparedstatements) {
		return mysql_upsert_prepared(dbh,s);
	} else {
		return mysql_upsert_raw(dbh,s);
	}
	return 1;
}

#endif
