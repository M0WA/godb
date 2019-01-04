#pragma once

#include "dbtypes.h"
#include "hooks.h"
#include "dbhandle.h"

/* *********** MySQL **************** */

#ifndef _DISABLE_MYSQL
#include <mysql/mysql.h>
struct _MySQLHandle {
	MYSQL *conn;
	MYSQL_STMT *stmt;
	MYSQL_RES *res;
} MySQLHandle;
#endif

/* *********** Postgres **************** */

#ifndef _DISABLE_POSTGRES
#include <postgresql/libpq-fe.h>
struct _PostgresHandle {
	PGconn *conn;
	PGresult *res;
	size_t resrow;
} PostgresHandle;
#endif

/* *********** DBI        **************** */

#ifndef _DISABLE_DBI
#include <dbi/dbi.h>
typedef struct _DBIHandle {
	dbi_conn conn;
	dbi_result result;
	const char *delimiter;
} DBIHandle;
#endif

/* *********** DBHandle **************** */

typedef struct _DBHandle {
	DBConfig config;
	DBCredentials cred;
	DBHooks hooks;

	union {
#ifndef _DISABLE_MYSQL
		struct _MySQLHandle mysql;
#endif
#ifndef _DISABLE_POSTGRES
		struct _PostgresHandle postgres;
#endif
#ifndef _DISABLE_DBI
		struct _DBIHandle dbi;
#endif
	};
} DBHandle;
