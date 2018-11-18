#ifndef _DISABLE_MYSQL

#include "mysqlhooks.h"

#include <string.h>

#include "dbhandle_impl.h"
#include "statements.h"
#include "column.h"
#include "logger.h"

int mysql_connect_hook(struct _DBHandle* dbh) {
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
	mysql_options(dbh->mysql.conn,MYSQL_OPT_RECONNECT,(const char*)&one);
	//mysql_options(dbh->mysql.conn,MYSQL_OPT_COMPRESS, (const char*)&one);

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

int mysql_disconnect_hook(struct _DBHandle* dbh) {
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

int mysql_insert_hook(struct _DBHandle* dbh,const struct _InsertStmt *const s) {
	int rc = 0;
	char colnames[1024] = {0};
	char stmtbuf[4096] = {0};
	MYSQL_STMT *mystmt = 0;
	MYSQL_BIND *bind = 0;
	char* values = 0;

	size_t valsize = (s->ncols * 2) + 3 + 1;
	values = malloc(valsize);
	if(!values) {
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}
	values[0] = 0;
	strcat(values,"(");

	for(size_t i = 0; i < s->nrows; i++) {
		for(size_t j = 0; j < s->ncols; j++) {
			if(j) {
				strcat(colnames,",");
				strcat(values,",");
			}
			strcat(colnames,s->defs[j].name);
			strcat(values,"?");
		}
		if(s->nrows > 1 && i < (s->nrows-1)) {
			strcat(values,",");
		}
	}
	strcat(values,")");

	char fmt[] = "INSERT INTO `%s`.`%s` (%s) VALUES %s";
	sprintf(stmtbuf,fmt,s->defs->database,s->defs->table,colnames,values);

	mystmt = mysql_stmt_init(dbh->mysql.conn);
	if(!mystmt) {
		rc = 1;
		LOGF_WARN("mysql_stmt_init: %s",mysql_error(dbh->mysql.conn));
		goto MYSQL_INSERT_EXIT; }

	int err = mysql_stmt_prepare(mystmt, stmtbuf, strlen(stmtbuf));
	if(err) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}

	bind = malloc(sizeof(MYSQL_BIND) * s->ncols);
	if(!bind) {
		rc = 1;
		goto MYSQL_INSERT_EXIT; }
	if( mysql_stmt_bind_param(mystmt,bind) ) {
		LOGF_WARN("mysql_bind_param: %s",mysql_error(dbh->mysql.conn));
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}

	for(size_t i = 0; i < s->nrows; i++) {
		for(size_t j = 0; j < s->ncols; j++) {

			//const void* tmp = get_colbuf_by_idx_dbcolumnsdefs(j,s->defs,s->ncols,s->valbuf[i]);
			//const unsigned char* valbuf = s->valbuf;
			//
		}
	}

MYSQL_INSERT_EXIT:
	if(bind) {
		free(bind);	}
	if(values) {
		free(values); }
	if(mystmt) {
		mysql_stmt_close(mystmt); }
	return rc;
}

int mysql_update_hook(struct _DBHandle* dbh,const struct _UpdateStmt *const s) {
	return 1;
}

int mysql_upsert_hook(struct _DBHandle* dbh,const struct _UpsertStmt *const s) {
	return 1;
}

int mysql_delete_hook(struct _DBHandle* dbh,const struct _DeleteStmt *const s) {
	return 1;
}

struct _SelectResult* mysql_select_hook(struct _DBHandle* dbh,const struct _SelectStmt *const s) {
	return 0;
}

#endif
