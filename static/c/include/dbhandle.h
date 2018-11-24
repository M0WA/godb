#pragma once

#include "dblimits.h"

struct _DBHandle;
typedef struct _DBHandle DBHandle;

typedef struct _DBCredentials {
	char host[MAX_DB_HOST];
	unsigned short port;
	char name[MAX_DB_NAME];
	char user[MAX_DB_USER];
	char pass[MAX_DB_PASS];
} DBCredentials;

#ifndef _DISABLE_MYSQL
struct _MySQLConfig {
	int autoreconnect;
	int compression;
} MySQLConfig;
#endif

#ifndef _DISABLE_DBI
#include "dbitypes.h"
typedef struct _DBIConfig {
	DBIType type;
} DBIConfig;
#endif

typedef struct _DBConfig {
	DBTypes type;
	union {
#ifndef _DISABLE_MYSQL
		struct _MySQLConfig mysql;
#endif
#ifndef _DISABLE_DBI
		struct _DBIConfig dbi;
#endif
	};
} DBConfig;
