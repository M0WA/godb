#ifndef _DISABLE_POSTGRES

#include "postgresfuncs.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "hooks.h"
#include "postgreshooks.h"

int postgres_init_dbh(struct _DBHandle* dbh) {
	if(PQisthreadsafe() == 0) {
		LOG_FATAL(1,"please use a thread-safe version of pq library"); }
	if(dbh->postgres.conn) {
		Log(LOG_WARN,"postgres handle is already initalized"); }
	REGISTER_HOOKS(dbh,postgres)
	return 1;
}

#endif
