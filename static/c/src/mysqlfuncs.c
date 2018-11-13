#ifndef _DISABLE_MYSQL

#include "mysqlfuncs.h"

#include "dbhandle_impl.h"
#include "hooks.h"
#include "mysqlhooks.h"

int mysql_init_dbh(struct _DBHandle* dbh) {
	REGISTER_HOOKS(dbh,mysql);
	return 1;
}

#endif
