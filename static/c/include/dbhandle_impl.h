#pragma once

#include "dbtypes.h"
#include "hooks.h"
#include "dblimits.h"

/* *********** MySQL **************** */

#ifndef _DISABLE_MYSQL
struct _MySQLHandle {

} MySQLHandle;
#endif

/* *********** Postgres **************** */

#ifndef _DISABLE_POSTGRES
struct _PostgresHandle {

} PostgresHandle;
#endif

/* *********** DBConfig **************** */

typedef struct _DBConfig {
	char host[MAX_DB_HOST];
	unsigned short port;
	char name[MAX_DB_NAME];
	char user[MAX_DB_USER];
	char pass[MAX_DB_PASS];
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
