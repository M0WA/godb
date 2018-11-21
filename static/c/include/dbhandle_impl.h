#pragma once

#include "dbtypes.h"
#include "hooks.h"
#include "dblimits.h"

/* *********** MySQL **************** */

#ifndef _DISABLE_MYSQL
#include <mysql/mysql.h>
struct _MySQLHandle {
	MYSQL *conn;
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
	PGconn *conn;
} PostgresHandle;
#endif

#ifndef _DISABLE_DBI
#include <dbi/dbi.h>
typedef enum _DBIType {
	DBI_TYPE_MYSQL,
} DBIType;

typedef struct _DBIHandle {
	dbi_inst inst;
	dbi_conn conn;
} DBIHandle;

typedef struct _DBIConfig {
	DBIType type;
} DBIConfig;
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
#ifndef _DISABLE_DBI
		struct _DBIConfig dbi;
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
#ifndef _DISABLE_DBI
		struct _DBIHandle dbi;
#endif
	};
} DBHandle;
