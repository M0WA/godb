#pragma once

#include <time.h>
#include <string.h>

#include "dbtypes.h"
#include "tables.h"

struct _DBHandle;
struct _DBTable;
typedef struct _DBHandle DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

/* db connection */

struct _DBHandle* create_dbhandle(DBTypes type);
int destroy_dbhandle(struct _DBHandle** dbh);
int connect_db(struct _DBHandle* dbh,const char* __restrict host,unsigned short port,const char* __restrict db,const char* __restrict user, const char* __restrict pass);
int disconnect_db(struct _DBHandle* dbh);

/* statements */
int insert_db(struct _DBHandle* dbh,struct _InsertStmt* stmt);
int update_db(struct _DBHandle* dbh,struct _UpdateStmt* stmt);
int upsert_db(struct _DBHandle* dbh,struct _UpsertStmt* stmt);
int delete_db(struct _DBHandle* dbh,struct _DeleteStmt* stmt);
int select_db(struct _DBHandle* dbh,struct _SelectStmt* stmt,struct _SelectResult** res);

/* tables */

void destroy_dbtable(struct _DBTable** tbl);

#define TABLE_STRUCT(dbname,table,var) \
	dbname##_##table var; \
	_create_##dbname##_##table( &var );

/* columns */

#define SET_COLUMN(var,colname,val) { \
	if(! var.colname ) { \
		var.colname = &var.valbuf.colname; } \
	*(var.colname) = val; \
}

#define SET_COLUMN_STRING(var,colname,val) { \
	if(! var.colname ) { \
		var.colname = (char*)&var.valbuf.colname; } \
	size_t l = sizeof(var.valbuf.colname) - 1; \
	if(strlen(val) < l) { \
		l = strlen(val); } \
	memcpy(var.valbuf.colname, val, l ); \
	var.valbuf.colname[l] = 0; \
}

#define SET_COLUMN_TM(var,colname,tval) { \
	if(! var.colname ) { \
		var.colname = &var.valbuf.colname; } \
	memcpy(&var.valbuf.colname, &tval, sizeof(struct tm) ); \
}

#define SET_COLUMN_NULL(var,colname) { \
	if( var.colname ) { \
		var.colname = 0; } \
}
