#pragma once

#include "dblimits.h"
#include "dbtypes.h"

struct _DBHandle;
typedef struct _DBHandle DBHandle;

typedef struct _DBCredentials {
	char Host[MAX_DB_HOST];
	unsigned short Port;
	char Name[MAX_DB_NAME];
	char User[MAX_DB_USER];
	char Pass[MAX_DB_PASS];
} DBCredentials;

#ifndef _DISABLE_MYSQL
typedef struct _MySQLConfig {
	int Autoreconnect;
	int Compression;
	int Preparedstatements;
} MySQLConfig;
#endif

#ifndef _DISABLE_POSTGRES
typedef struct _PostgresConfig {
	int Preparedstatements;
} PostgresConfig;
#endif

#ifndef _DISABLE_DBI
#include "dbitypes.h"
typedef struct _DBIConfig {
	DBIType Type;
} DBIConfig;
#endif

typedef struct _DBConfig {
	DBTypes Type;
	union {
#ifndef _DISABLE_MYSQL
		struct _MySQLConfig Mysql;
#endif
#ifndef _DISABLE_POSTGRES
		struct _PostgresConfig Postgres;
#endif
#ifndef _DISABLE_DBI
		struct _DBIConfig Dbi;
#endif
	};
} DBConfig;

void dump_dbconfig(const struct _DBConfig *conf);
