#ifndef _DISABLE_POSTGRES

#include "postgresfuncs.h"

#include "dbhandle_impl.h"
#include "hooks.h"
#include "postgreshooks.h"

int postgres_init_dbh(struct _DBHandle* dbh) {
	REGISTER_HOOKS(dbh,postgres);
	return 1;
}

#endif
