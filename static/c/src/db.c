#include "db.h"

#include "dbhandle_impl.h"
#include "logger.h"
#include "table.h"
#include "statements.h"
#include "selectresult.h"

#ifndef _DISABLE_MYSQL
	#include "mysqlhooks.h"
#endif
#ifndef _DISABLE_POSTGRES
	#include "postgreshooks.h"
#endif
#ifndef _DISABLE_DBI
	#include "dbihooks.h"
#endif

#include <stdlib.h>
#include <string.h>

static int initialized = 0;

static int init_db_credentials(struct _DBHandle *dbh,const struct _DBCredentials *cred) {
	if(!cred->Host || strlen(cred->Host) > (MAX_DB_HOST - 1) ) {
		LOG_ERROR("hostname too long");
		return 0;
	}
	if(!cred->Port) {
		LOG_ERROR("invalid port");
		return 0;
	}
	if(!cred->Name || strlen(cred->Name) > (MAX_DB_NAME - 1) ) {
		LOG_ERROR("database name too long");
		return 0;
	}
	if(!cred->User || strlen(cred->User) > (MAX_DB_USER - 1) ) {
		LOG_ERROR("username too long");
		return 0;
	}
	if(!cred->Pass || strlen(cred->Pass) > (MAX_DB_PASS - 1) ) {
		LOG_ERROR("password too long");
		return 0;
	}

	dbh->cred.Port = cred->Port;
	strncpy( dbh->cred.Host, cred->Host, MAX_DB_HOST );
	strncpy( dbh->cred.Name, cred->Name, MAX_DB_NAME );
	strncpy( dbh->cred.User, cred->User, MAX_DB_USER );
	strncpy( dbh->cred.Pass, cred->Pass, MAX_DB_PASS );
	return 0;
}

int init_dblib() {
	int rc = 0;
	if(!initialized) {
		LOG_DEBUG("initialize database library");
#ifndef _DISABLE_MYSQL
		rc &= mysql_initlib_hook();
#endif
#ifndef _DISABLE_POSTGRES
		rc &= postgres_initlib_hook();
#endif
#ifndef _DISABLE_DBI
		rc &= dbi_initlib_hook();
#endif
		initialized = 1;
	}
	return rc;
}

int exit_dblib() {
	LOG_DEBUG("release database library");
#ifndef _DISABLE_MYSQL
	mysql_exitlib_hook();
#endif
#ifndef _DISABLE_POSTGRES
	postgres_exitlib_hook();
#endif
#ifndef _DISABLE_DBI
	dbi_exitlib_hook();
#endif
	logger_end();
	return 0;
}

DBHandle *create_dbhandle(const struct _DBConfig *conf) {
	if(!initialized && init_dblib()) {
		return 0; }

	DBHandle *dbh = malloc(sizeof(DBHandle));
	if(!dbh && !conf) {
		LOG_ERROR("could not malloc database handle or null config");
		return 0;
	}
	memset(dbh,0,sizeof(DBHandle));
	dbh->config = *conf;

	switch(dbh->config.Type) {
#ifndef _DISABLE_MYSQL
	case DB_TYPE_MYSQL:
		REGISTER_HOOKS(dbh,mysql);
		break;
#endif
#ifndef _DISABLE_POSTGRES
	case DB_TYPE_POSTGRES:
		REGISTER_HOOKS(dbh,postgres);
		break;
#endif
#ifndef _DISABLE_DBI
	case DB_TYPE_DBI:
		REGISTER_HOOKS(dbh,dbi);
		break;
#endif
	default:
		free(dbh);
		LOG_ERROR("invalid database type");
		return 0;
	}

	if( !dbh->hooks.create || dbh->hooks.create(dbh) ) {
		free(dbh);
		LOG_ERROR("could not initialize db handle");
		return 0;
	}
	return dbh;
}

int connect_db(struct _DBHandle *dbh,const struct _DBCredentials *cred) {
	if( init_db_credentials(dbh,cred) ) {
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

int destroy_dbhandle(struct _DBHandle *dbh) {
	if(!dbh) {
		return 1;
	}
	if( !dbh->hooks.destroy || dbh->hooks.destroy(dbh) ) {
		free(dbh);
		LOG_ERROR("could not destroy db handle");
		return 0;
	}
	free(dbh);
	return 0;
}

int insert_db(struct _DBHandle *dbh,const struct _InsertStmt *const stmt) {
	if(!dbh) {
		LOG_ERROR("null database handle/table/values");
		return 1;
	}
	if(!dbh->hooks.insert) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	return dbh->hooks.insert(dbh,stmt);
}

int update_db(struct _DBHandle *dbh,const struct _UpdateStmt *const stmt) {
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

int upsert_db(struct _DBHandle *dbh,const struct _UpsertStmt *const stmt) {
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

int delete_db(struct _DBHandle *dbh,const struct _DeleteStmt *const stmt) {
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

int select_db(struct _DBHandle *dbh,const struct _SelectStmt *const stmt, struct _SelectResult* res) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.select) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	if(create_selectresult(stmt, res)) {
		LOG_ERROR("cannot create select result");
		return 1;
	}
	return dbh->hooks.select(dbh,stmt,res);
}

int fetch_db(struct _DBHandle *dbh,struct _SelectResult* res) {
	if(!dbh) {
		LOG_ERROR("null database handle");
		return 1;
	}
	if(!dbh->hooks.fetch) {
		LOG_ERROR("invalid database handle");
		return 1;
	}
	return dbh->hooks.fetch(dbh,res);
}

void dump_dbconfig(const struct _DBConfig *conf) {
	switch(conf->Type) {
	case DB_TYPE_MYSQL:
		LOG_DEBUG("dbconfig type: MYSQL");
		break;
	case DB_TYPE_POSTGRES:
		LOG_DEBUG("dbconfig type: POSTGRES");
		break;
	case DB_TYPE_DBI:
		LOG_DEBUG("dbconfig type: DBI");
		switch(conf->Dbi.Type) {
		case DBI_TYPE_MYSQL:
			LOG_DEBUG("dbconfig dbitype: MYSQL");
			break;
		case DBI_TYPE_POSTGRES:
			LOG_DEBUG("dbconfig dbitype: POSTGRE");
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
