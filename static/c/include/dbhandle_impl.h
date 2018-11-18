#pragma once

#include "dbtypes.h"
#include "hooks.h"
#include "dblimits.h"

/* *********** MySQL **************** */

#ifndef _DISABLE_MYSQL
#include <mysql/mysql.h>
struct _MySQLHandle {
	MYSQL* conn;
} MySQLHandle;

struct _MySQLConfig {
	int autoreconnect;
	int compression;
} MySQLConfig;
#endif

/* *********** Postgres **************** */

#ifndef _DISABLE_POSTGRES
#include <postgresql/libpq-fe.h>
struct _PostgresHandle {
	PGconn* conn;
} PostgresHandle;
#endif

/* *********** DBConfig **************** */

typedef struct _DBConfig {
	char host[MAX_DB_HOST];
	unsigned short port;
	char name[MAX_DB_NAME];
	char user[MAX_DB_USER];
	char pass[MAX_DB_PASS];

#ifndef _DISABLE_MYSQL
	struct _MySQLConfig mysql;
#endif
} DBConfig;

/* *********** DBHandle **************** */

typedef struct _DBHandle {
	DBTypes type;
	DBConfig config;
	DBHooks hooks;

	union {
#ifndef _DISABLE_MYSQL
		struct _MySQLHandle mysql;
#endif
#ifndef _DISABLE_POSTGRES
		struct _PostgresHandle postgres;
#endif
	};

} DBHandle;
