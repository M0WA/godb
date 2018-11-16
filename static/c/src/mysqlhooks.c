#ifndef _DISABLE_MYSQL

#include "mysqlhooks.h"

#include "dbhandle_impl.h"
#include "logger.h"

int mysql_connect_hook(struct _DBHandle* dbh) {
	if(!dbh) {
		Log(LOG_WARN,"handle is null");
		return 1;
	}
	if(dbh->mysql.conn) {
		Log(LOG_WARN,"mysql handle is already initalized");
		return 1;
	}
	if(! (dbh->mysql.conn = mysql_init(dbh->mysql.conn)) ) {
		Log(LOG_WARN,"mysql handle failed to initalize");
		return 1;
	}

	my_bool one = 1;
	mysql_options(dbh->mysql.conn,MYSQL_OPT_RECONNECT,(const char*)&one);
	//mysql_options(dbh->mysql.conn,MYSQL_OPT_COMPRESS, (const char*)&one);

	MYSQL* tmp = dbh->mysql.conn;
	dbh->mysql.conn = mysql_real_connect(
			dbh->mysql.conn,
			dbh->config.host,
			dbh->config.user,
			dbh->config.pass,
			"",
			dbh->config.port,
			NULL,
			CLIENT_MULTI_STATEMENTS |
			CLIENT_COMPRESS |
			CLIENT_IGNORE_SIGPIPE );

	if(!dbh->mysql.conn) {
		Logf(LOG_WARN,"mysql connect: %s",mysql_error(tmp));
		return 1;
	}
	mysql_autocommit(dbh->mysql.conn,1);

	Log(LOG_DEBUG,"connected to mysql-database");
	return 1;
}

int mysql_disconnect_hook(struct _DBHandle* dbh) {
	if(!dbh) {
		Log(LOG_WARN,"handle is null");
		return 1;
	}
	if(!dbh->mysql.conn) {
		Log(LOG_WARN,"mysql handle is not initalized");
		return 1;
	}
	mysql_close(dbh->mysql.conn);
	dbh->mysql.conn = 0;

	Log(LOG_DEBUG,"disconnected from mysql-database");
	return 1;
}

int mysql_insert_hook(struct _DBHandle* dbh,struct _InsertStmt* s) {
	return 1;
}

int mysql_update_hook(struct _DBHandle* dbh,struct _UpdateStmt* s) {
	return 1;
}

int mysql_upsert_hook(struct _DBHandle* dbh,struct _UpsertStmt* s) {
	return 1;
}

int mysql_delete_hook(struct _DBHandle* dbh,struct _DeleteStmt* s) {
	return 1;
}

struct _SelectResult* mysql_select_hook(struct _DBHandle* dbh,struct _SelectStmt* s) {
	return 0;
}

#endif
