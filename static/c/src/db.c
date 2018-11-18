#include "db.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "table.h"
#include "statements.h"

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
		LOG_ERROR("hostname too long");
		return 0;
	}
	if(!port) {
		LOG_ERROR("invalid port");
		return 0;
	}
	if(!db || strlen(db) > (MAX_DB_NAME - 1) ) {
		LOG_ERROR("database name too long");
		return 0;
	}
	if(!user || strlen(user) > (MAX_DB_USER - 1) ) {
		LOG_ERROR("username too long");
		return 0;
	}
	if(!pass || strlen(pass) > (MAX_DB_PASS - 1) ) {
		LOG_ERROR("password too long");
		return 0;
	}

	conf->port = port;
	strncpy( conf->host, host, MAX_DB_HOST );
	strncpy( conf->name, db  ,  MAX_DB_NAME );
	strncpy( conf->user, user, MAX_DB_USER );
	strncpy( conf->pass, pass, MAX_DB_PASS );

#ifndef _DISABLE_MYSQL
	conf->mysql.autoreconnect = 1;
	conf->mysql.compression = 1;
#endif
	return 0;
}

DBHandle* create_dbhandle(DBTypes type) {
	DBHandle* dbh = malloc(sizeof(DBHandle));
	if(!dbh) {
		LOG_ERROR("could not malloc database handle");
		return 0;
	}
	memset(dbh,0,sizeof(DBHandle));

	switch(type) {
#ifndef _DISABLE_MYSQL
	case DB_TYPE_MYSQL:
		if( mysql_init_dbh(dbh) ) {
			free(dbh);
			LOG_ERROR("could not init mysql database handle");
			return 0;
		}
		break;
#endif
#ifndef _DISABLE_POSTGRES
	case DB_TYPE_POSTGRES:
		if( postgres_init_dbh(dbh) ) {
			free(dbh);
			LOG_ERROR("could not init postgres database handle");
			return 0;
		}
		break;
#endif
	default:
		free(dbh);
		LOG_ERROR("invalid database type");
		return 0;
	}
	return dbh;
}

int connect_db(DBHandle* dbh,const char* __restrict host,unsigned short port,const char* __restrict db,const char* __restrict user, const char* __restrict pass) {
	if( init_db_config(&(dbh->config),host,port,db,user,pass) ) {
		return 1;
	}
	if(!dbh->hooks.connect) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	return dbh->hooks.connect(dbh);
}

int disconnect_db(DBHandle* dbh) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.disconnect) {
		LOG_ERROR("invalid database handle");
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

int insert_stmt(struct _DBHandle* dbh,const struct _InsertStmt *const stmt) {
	if(!dbh || !dbh->hooks.insert || !stmt) {
		LOG_ERROR("null database handle/hook/stmt");
		return 1;
	}
	return dbh->hooks.insert(dbh,stmt);
}

int insert_dbtable(struct _DBHandle* dbh,const struct _DBTable *const*const tbl,size_t nrows) {
	if(!dbh || !tbl || !nrows) {
		LOG_ERROR("null database handle/dbtable");
		return 1;
	}
	void*** buf = malloc(sizeof(void**) * nrows);
	if(!buf) {
		return 1; }
	for(size_t i = 0; i < nrows; i++) {
		buf[i] = tbl[i]->valbuf;
	}
	int rc = insert_db(dbh,tbl[0]->def,(const void *const*const*const)buf,nrows);
	free(buf);
	return rc;
}

int insert_db(struct _DBHandle* dbh,const struct _DBTableDef *const def,const void *const*const*const values,size_t nrows){
	if(!dbh|!def|!values) {
		LOG_ERROR("null database handle/table/values");
		return 1;
	}
	struct _InsertStmt stmt = (struct _InsertStmt) {
		.defs = def->cols,
		.ncols = def->ncols,
		.valbuf = values,
		.nrows = nrows,
	};
	return insert_stmt(dbh,&stmt);
}

int update_db(struct _DBHandle* dbh,const struct _UpdateStmt *const stmt) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.update) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	return dbh->hooks.update(dbh,stmt);
}

int upsert_db(struct _DBHandle* dbh,const struct _UpsertStmt *const stmt) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.upsert) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	return dbh->hooks.upsert(dbh,stmt);
}

int delete_db(struct _DBHandle* dbh,const struct _DeleteStmt *const stmt) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.delete) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	return dbh->hooks.delete(dbh,stmt);
}

int select_db(struct _DBHandle* dbh,const struct _SelectStmt *const stmt,const struct _SelectResult**const res) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.select) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	*res = dbh->hooks.select(dbh,stmt);
	return 0;
}
