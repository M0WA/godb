#include "db.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "table.h"

#ifndef _DISABLE_MYSQL
	#include "mysqlfuncs.h"
#endif
#ifndef _DISABLE_POSTGRES
	#include "postgresfuncs.h"
#endif

#include <stdlib.h>
#include <string.h>

static int init_db_config(DBConfig* conf,const char* __restrict host,unsigned short port,const char* __restrict db,const char* __restrict user, const char* __restrict pass) {
	if(!host || strlen(host) > (MAX_DB_HOST - 1) ) {
		Log(LOG_ERROR,"hostname too long");
		return 0;
	}
	if(!port) {
		Log(LOG_ERROR,"invalid port");
		return 0;
	}
	if(!db || strlen(db) > (MAX_DB_NAME - 1) ) {
		Log(LOG_ERROR,"database name too long");
		return 0;
	}
	if(!user || strlen(user) > (MAX_DB_USER - 1) ) {
		Log(LOG_ERROR,"username too long");
		return 0;
	}
	if(!pass || strlen(pass) > (MAX_DB_PASS - 1) ) {
		Log(LOG_ERROR,"password too long");
		return 0;
	}

	conf->port = port;
	strncpy( conf->host, host, MAX_DB_HOST );
	strncpy( conf->name, db  ,  MAX_DB_NAME );
	strncpy( conf->user, user, MAX_DB_USER );
	strncpy( conf->pass, pass, MAX_DB_PASS );
	return 0;
}

DBHandle* create_dbhandle(DBTypes type) {
	DBHandle* dbh = malloc(sizeof(DBHandle));
	if(!dbh) {
		Log(LOG_ERROR,"could not malloc database handle");
		return 0;
	}
	memset(dbh,0,sizeof(DBHandle));

	switch(type) {
#ifndef _DISABLE_MYSQL
	case DB_TYPE_MYSQL:
		if( mysql_init_dbh(dbh) ) {
			free(dbh);
			Log(LOG_ERROR,"could not init mysql database handle");
			return 0;
		}
		break;
#endif
#ifndef _DISABLE_POSTGRES
	case DB_TYPE_POSTGRES:
		if( postgres_init_dbh(dbh) ) {
			free(dbh);
			Log(LOG_ERROR,"could not init postgres database handle");
			return 0;
		}
		break;
#endif
	default:
		free(dbh);
		Log(LOG_ERROR,"invalid database type");
		return 0;
	}
	return dbh;
}

int connect_db(DBHandle* dbh,const char* __restrict host,unsigned short port,const char* __restrict db,const char* __restrict user, const char* __restrict pass) {
	if( init_db_config(&(dbh->config),host,port,db,user,pass) ) {
		return 1;
	}
	if(!dbh->hooks.connect) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	return dbh->hooks.connect(dbh);
}

int disconnect_db(DBHandle* dbh) {
	if(!dbh) {
		Log(LOG_ERROR,"null database handle");
		return 1;
	}
	if(!dbh->hooks.disconnect) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	return dbh->hooks.disconnect(dbh);
}

int destroy_dbhandle(struct _DBHandle** dbh) {
	if(!dbh || !*dbh) {
		return 1;
	}
	free(*dbh);
	*dbh = 0;
	return 0;
}

void destroy_dbtable(struct _DBTable** tbl) {
	if(!tbl||!*tbl) {
		return; }
	if(!(*tbl)->valbuf) {
		return; }
	free((*tbl)->valbuf);
	(*tbl)->valbuf = 0;
}

int insert_db(struct _DBHandle* dbh,struct _InsertStmt* stmt) {
	if(!dbh) {
		Log(LOG_ERROR,"null database handle");
		return 1;
	}
	if(!dbh->hooks.insert) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	return dbh->hooks.insert(dbh,stmt);
}

int update_db(struct _DBHandle* dbh,struct _UpdateStmt* stmt) {
	if(!dbh) {
		Log(LOG_ERROR,"null database handle");
		return 1;
	}
	if(!dbh->hooks.update) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	return dbh->hooks.update(dbh,stmt);
}

int upsert_db(struct _DBHandle* dbh,struct _UpsertStmt* stmt) {
	if(!dbh) {
		Log(LOG_ERROR,"null database handle");
		return 1;
	}
	if(!dbh->hooks.upsert) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	return dbh->hooks.upsert(dbh,stmt);
}

int delete_db(struct _DBHandle* dbh,struct _DeleteStmt* stmt) {
	if(!dbh) {
		Log(LOG_ERROR,"null database handle");
		return 1;
	}
	if(!dbh->hooks.delete) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	return dbh->hooks.delete(dbh,stmt);
}

int select_db(struct _DBHandle* dbh,struct _SelectStmt* stmt,struct _SelectResult** res) {
	if(!dbh) {
		Log(LOG_ERROR,"null database handle");
		return 1;
	}
	if(!dbh->hooks.select) {
		Log(LOG_ERROR,"invalid database handle");
		return 1;
	}
	*res = dbh->hooks.select(dbh,stmt);
	return 0;
}
