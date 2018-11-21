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
	MYSQL_STMT *mystmt = 0;

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

	mystmt = mysql_stmt_init(dbh->mysql.conn);
	if(!mystmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_INSERT_EXIT; }

	if( mysql_stmt_prepare(mystmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_param_count(mystmt) != (s->ncols * s->nrows) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOG_WARN("mysql_stmt_param_count: invalid column count in prepared statement");
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_bind_param(mystmt,bind.bind) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}
	if( mysql_stmt_execute(mystmt) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}

MYSQL_INSERT_EXIT:
	if(mystmt) {
		mysql_stmt_close(mystmt); }
	if(colnames) {
		free(colnames);	}
	if(values) {
		free(values); }

	return rc;
}

int mysql_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult** res) {
	const char fmt[] = "SELECT %s FROM `%s`.`%s` %s %s %s";
	char *colnames = 0;
	char *where = 0;
	char *stmtbuf = 0;
	char limit[32] = {0};
	int rc = 0;
	MYSQL_STMT *mystmt = 0;

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	if( mysql_where(&s->where,&bind,&where) ) {
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

	mystmt = mysql_stmt_init(dbh->mysql.conn);
	if(!mystmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_SELECT_EXIT; }
	if( mysql_stmt_prepare(mystmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}
	if( bind.bind_idx && mysql_stmt_bind_param(mystmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}
	if( mysql_stmt_execute(mystmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	if( mysql_selectresult_from_stmt(s->defs,s->ncols,res,mystmt) ) {
		rc = 1;
		goto MYSQL_SELECT_EXIT;}

MYSQL_SELECT_EXIT:
	if(mystmt) {
		mysql_stmt_close(mystmt); }
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
	MYSQL_STMT *mystmt = 0;

	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	const char fmt[] = "DELETE FROM `%s`.`%s` %s %s %s";
	size_t wheresize = where ? strlen(where) : 0;
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->def->database) + strlen(s->def->table) + 1;
	stmtbuf = alloca(sqlsize);
	if(!stmtbuf) {
		goto MYSQL_DELETE_EXIT;
		rc = 1; }
	sprintf(stmtbuf,fmt,s->def->database,s->def->table,(where ? " WHERE " : ""),(where ? where : ""),limit);

	mystmt = mysql_stmt_init(dbh->mysql.conn);
	if(!mystmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_DELETE_EXIT; }
	if( mysql_stmt_prepare(mystmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_DELETE_EXIT;	}
	if( bind.bind_idx && mysql_stmt_bind_param(mystmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_DELETE_EXIT;	}
	if( mysql_stmt_execute(mystmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_DELETE_EXIT;	}

MYSQL_DELETE_EXIT:
	if(where) {
		free(where); }
	return rc;
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
