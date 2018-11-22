#ifndef _DISABLE_MYSQL

#include "mysqlfuncs.h"

#include <stdlib.h>

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>

#include "dbhandle_impl.h"
#include "hooks.h"
#include "mysqlhooks.h"
#include "logger.h"

int mysql_init_dblib() {
	if (!mysql_thread_safe()) {
		LOG_FATAL(1,"please use a thread-safe version of mysqlclient library");	}
	return mysql_library_init(0, NULL, NULL);
}

int mysql_exit_dblib() {
	mysql_library_end();
	return 0;
}

int mysql_init_dbh(struct _DBHandle *dbh) {
	if(dbh->mysql.conn) {
		LOG_WARN("mysql handle is already initalized");	}
	dbh->mysql.stmt = 0;
	REGISTER_HOOKS(dbh,mysql)
	return 0;
}

#endif
