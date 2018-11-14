#pragma once

#include "dbtypes.h"

struct _DBHandle;
typedef struct _DBHandle DBHandle;

struct _DBHandle* create_dbhandle(DBTypes type);
int destroy_dbhandle(struct _DBHandle** dbh);
int connect_db(struct _DBHandle* dbh,const char* __restrict host,unsigned short port,const char* __restrict db,const char* __restrict user, const char* __restrict pass);
int disconnect_db(struct _DBHandle* dbh);

#define TABLE_STRUCT(dbname,table,var) ({ \
	_create_##dbname##_##table(&var); \
});
