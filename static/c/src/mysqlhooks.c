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
#include "selectresult.h"

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
			dbh->config.host,
			dbh->config.user,
			dbh->config.pass,
			dbh->config.name,
			dbh->config.port,
			NULL,
			CLIENT_MULTI_STATEMENTS |
			CLIENT_COMPRESS |
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

int mysql_insert_hook(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	int rc = 0;
	char *colnames = 0;
	char *stmtbuf = 0;
	char *values = 0;

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}

	for(size_t i = 0; i < s->nrows; i++) {
		if(i) {
			append_string(",",&values);	}
		append_string("(",&values);
		for(size_t j = 0; j < s->ncols; j++) {
			if( mysql_bind_append(&(s->defs[j]),s->valbuf[i][j], &bind ) ) {
				rc = 1;
				LOG_WARN("mysql_bind_append: error");
				goto MYSQL_INSERT_EXIT;
			}
			if(j) {
				append_string(",",&values);
			}
			append_string("?",&values);
		}
		append_string(")",&values);
	}

	char fmt[] = "INSERT INTO `%s`.`%s` (%s) VALUES %s";
	size_t lenStmt = 1 + (values ? strlen(values) : 0) + strlen(fmt) + strlen(colnames) + strlen(s->defs->database) + strlen(s->defs->table);
	stmtbuf = alloca(lenStmt);
	if(!stmtbuf) {
		return 1; }
	snprintf(stmtbuf,lenStmt,fmt,s->defs->database,s->defs->table,colnames,values);

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_prepare(dbh->mysql.stmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_param_count(dbh->mysql.stmt) != (s->ncols * s->nrows) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOG_WARN("mysql_stmt_param_count: invalid column count in prepared statement");
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}

MYSQL_INSERT_EXIT:
	if(dbh->mysql.stmt) {
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;}
	if(colnames) {
		free(colnames);	}
	if(values) {
		free(values); }

	return rc;
}

int mysql_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult *res) {
	const char fmt[] = "SELECT %s FROM `%s`.`%s` %s %s %s";
	char *colnames = 0;
	char *where = 0;
	char *stmtbuf = 0;
	char limit[32] = {0};
	int rc = 0;

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	if( mysql_where(&s->where,&bind) ) {
		rc = 1;
		goto MYSQL_SELECT_EXIT; }

	if(where_string(&s->where,"?",&where)) {
		rc = 1;
		goto MYSQL_SELECT_EXIT; }

	get_limit(s->limit, limit);

	size_t wheresize = where ? strlen(where) : 0;
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->defs[0].database) + strlen(s->defs[0].table) + strlen(colnames) + 1;
	stmtbuf = alloca(sqlsize);
	if(!stmtbuf) {
		rc = 1;
		goto MYSQL_SELECT_EXIT; }
	sprintf(stmtbuf,fmt,colnames,s->defs[0].database,s->defs[0].table,(where ? " WHERE " : ""),(where ? where : ""),limit);
	LOGF_DEBUG("statement:\n%s",stmtbuf);

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_SELECT_EXIT; }
	if( mysql_stmt_prepare(dbh->mysql.stmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}
	if( bind.bind_idx && mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	res->cols = s->defs;
	res->ncols = s->ncols;
	res->row = malloc(sizeof(void*) * s->ncols);
	if(!res->row) {
		return 1; }
	memset(res->row,0,sizeof(void*) * s->ncols);
	for(size_t i = 0; i < s->ncols; i++) {
		size_t colsize = mysql_get_colbuf_size(&(s->defs[i]));
		if(colsize == 0) {
			LOG_WARN("invalid column size");
			rc = 1;
			goto MYSQL_SELECT_EXIT; }
		res->row[i] = malloc(colsize);
	}

MYSQL_SELECT_EXIT:
	if(colnames) {
		free(colnames); }
	if(where) {
		free(where); }
	return rc;
}

int mysql_delete_hook(struct _DBHandle *dbh,const struct _DeleteStmt *const s) {
	char *where = 0;
	char *stmtbuf = 0;
	char limit[32] = {0};
	int rc = 0;

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	get_limit(s->limit, limit);

	if( mysql_where(&s->where,&bind) ) {
		rc = 1;
		goto MYSQL_DELETE_EXIT; }

	if(where_string(&s->where,"?",&where)) {
		rc = 1;
		goto MYSQL_DELETE_EXIT; }

	const char fmt[] = "DELETE FROM `%s`.`%s` %s %s %s";
	size_t wheresize = where ? strlen(where) : 0;
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->def->database) + strlen(s->def->table) + 1;
	stmtbuf = alloca(sqlsize);
	if(!stmtbuf) {
		goto MYSQL_DELETE_EXIT;
		rc = 1; }
	sprintf(stmtbuf,fmt,s->def->database,s->def->table,(where ? " WHERE " : ""),(where ? where : ""),limit);

	dbh->mysql.stmt = mysql_stmt_init(dbh->mysql.conn);
	if(!dbh->mysql.stmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_DELETE_EXIT; }
	if( mysql_stmt_prepare(dbh->mysql.stmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_DELETE_EXIT;	}
	if( bind.bind_idx && mysql_stmt_bind_param(dbh->mysql.stmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_DELETE_EXIT;	}
	if( mysql_stmt_execute(dbh->mysql.stmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(dbh->mysql.stmt));
		rc = 1;
		goto MYSQL_DELETE_EXIT;	}

MYSQL_DELETE_EXIT:
	if(where) {
		free(where); }
	if(dbh->mysql.stmt) {
		mysql_stmt_close(dbh->mysql.stmt);
		dbh->mysql.stmt = 0;}
	return rc;
}

int mysql_fetch_hook(struct _DBHandle *dbh,struct _SelectResult *res) {
	MYSQL_BIND bind[MAX_MYSQL_BIND_COLS];
	my_bool is_null[MAX_MYSQL_BIND_COLS];
	unsigned long length[MAX_MYSQL_BIND_COLS];
	MYSQL_TIME times[MAX_MYSQL_BIND_COLS];

	if(!res || !res->ncols || !dbh || !dbh->mysql.stmt || res->ncols > MAX_MYSQL_BIND_COLS) {
		destroy_selectresult(res);
		return -1;
	}

	memset(&bind,0,sizeof(MYSQL_BIND) * MAX_MYSQL_BIND_COLS);
	memset(&is_null,0,sizeof(my_bool) * MAX_MYSQL_BIND_COLS);
	memset(&length,0,sizeof(unsigned long) * MAX_MYSQL_BIND_COLS);

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

int mysql_update_hook(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	//const char fmt[] = "UPDATE `%s`.`%s` SET";
	return 1;
}

int mysql_upsert_hook(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	//const char fmt[] = "INSERT INTO `%s`.`%s` (%s) VALUES(%s) ON DUPLICATE KEY UPDATE %s";
	return 1;
}

#endif
