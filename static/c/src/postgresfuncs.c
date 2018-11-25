#ifndef _DISABLE_POSTGRES

#include "postgresfuncs.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "hooks.h"
#include "postgreshooks.h"

int postgres_init_dbh(struct _DBHandle *dbh) {
	if(dbh->postgres.conn) {
		LOG_WARN("postgres handle is already initalized"); }
	dbh->postgres.res = 0;
	REGISTER_HOOKS(dbh,postgres)
	return 0;
}

int postgres_init_dblib() {
	if(PQisthreadsafe() == 0) {
		LOG_FATAL(1,"please use a thread-safe version of pq library"); }
	return 0;
}

int postgres_exit_dblib() {
	return 0;
}

#endif
