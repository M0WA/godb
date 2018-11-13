#pragma once

#include "dbtypes.h"

struct _DBHandle;
typedef struct _DBHandle DBHandle;

struct _DBHandle* create_dbhandle(DBTypes type);
int destroy_dbhandle(struct _DBHandle** dbh);
int connect_db(struct _DBHandle* dbh,char* __restrict host,unsigned short port,char* __restrict db,char* __restrict user, char* __restrict pass);
int disconnect_db(struct _DBHandle* dbh);
