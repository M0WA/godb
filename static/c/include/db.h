#pragma once

#include <time.h>
#include <string.h>

#include "dbtypes.h"
#include "tables.h"

struct _DBHandle;
typedef struct _DBHandle DBHandle;
struct _DBConfig;
typedef struct _DBConfig DBConfig;
struct _DBCredentials;
typedef struct _DBCredentials DBCredentials;

struct _DBTable;
struct _DBTableDef;
struct _DBColumnDef;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

/* (de-)initialization */
int init_dblib();
int exit_dblib();

/* db connection */
struct _DBHandle* create_dbhandle(const struct _DBConfig *conf);
int destroy_dbhandle(struct _DBHandle *dbh);
int connect_db(struct _DBHandle *dbh,const struct _DBCredentials *cred);
int disconnect_db(struct _DBHandle *dbh);

/* statements */
int insert_db(struct _DBHandle *dbh,const struct _InsertStmt *const stmt);
int update_db(struct _DBHandle *dbh,const struct _UpdateStmt *const stmt);
int upsert_db(struct _DBHandle *dbh,const struct _UpsertStmt *const stmt);
int delete_db(struct _DBHandle *dbh,const struct _DeleteStmt *const stmt);
int select_db(struct _DBHandle *dbh,const struct _SelectStmt *const stmt, struct _SelectResult* res);
int fetch_db(struct _DBHandle *dbh,struct _SelectResult* res);

#define INSERT_ONE_DBTABLE(_handle,_dbtbl) ({ \
	const struct _DBTable *const t = _dbtbl; \
	int rc = insert_dbtable(_handle,&t,1); \
	rc; \
})

/* tables */
#define INIT_TABLE(dbname,table,var) \
	dbname##_##table var; \
	create_##dbname##_##table( &var, 1 );

#define INIT_TABLE_ROWS(dbname,table,var,rcnt) \
	dbname##_##table var; \
	create_##dbname##_##table( &var, rcnt );
