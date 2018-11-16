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

int mysql_init_dbh(struct _DBHandle* dbh) {
	if (!mysql_thread_safe()) {
		LOG_FATAL(1,"please use a thread-safe version of mysqlclient library");	}
	if(dbh->mysql.conn) {
		Log(LOG_WARN,"mysql handle is already initalized");	}
	REGISTER_HOOKS(dbh,mysql)
	return 0;
}

#endif
