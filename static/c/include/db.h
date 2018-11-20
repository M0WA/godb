#pragma once

#include <time.h>
#include <string.h>

#include "dbtypes.h"
#include "tables.h"

struct _DBHandle;
typedef struct _DBHandle DBHandle;

struct _DBTable;
struct _DBTableDef;
struct _DBColumnDef;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

/* db connection */

int init_dblib();
int exit_dblib();
struct _DBHandle* create_dbhandle(DBTypes type);
int destroy_dbhandle(struct _DBHandle** dbh);
int connect_db(struct _DBHandle *dbh,const char* __restrict host,unsigned short port,const char* __restrict db,const char* __restrict user, const char* __restrict pass);
int disconnect_db(struct _DBHandle *dbh);

/* statements */
int insert_dbtable(struct _DBHandle *dbh,const struct _DBTable *const*const tbl,size_t nrows);
int insert_db(struct _DBHandle *dbh,const struct _DBTableDef *const def,const void *const*const*const values,size_t nrows);
int insert_stmt(struct _DBHandle *dbh,const struct _InsertStmt *const stmt);

int update_db(struct _DBHandle *dbh,const struct _UpdateStmt *const stmt);
int upsert_db(struct _DBHandle *dbh,const struct _UpsertStmt *const stmt);
int delete_db(struct _DBHandle *dbh,const struct _DeleteStmt *const stmt);
int select_db(struct _DBHandle *dbh,const struct _SelectStmt *const stmt, struct _SelectResult** res);

#define INSERT_ONE_DBTABLE(_handle,_dbtbl) ({ \
	const struct _DBTable *const t = _dbtbl; \
	int rc = insert_dbtable(_handle,&t,1); \
	rc; \
})

/* tables */

void destroy_dbtable(struct _DBTable** tbl);

#define TABLE_STRUCT(dbname,table,var) \
	dbname##_##table var; \
	_create_##dbname##_##table( &var );

/* columns */

#define SET_COLUMN(var,colname,val) { \
	if(! var.tbl.colptr.colname ) { \
		var.tbl.colptr.colname = &var.tbl.valbuf.colname; } \
	*(var.tbl.colptr.colname) = val; \
}

#define SET_COLUMN_STRING(var,colname,val) { \
	if(! var.tbl.colptr.colname ) { \
		var.tbl.colptr.colname = (char*)&var.tbl.valbuf.colname; } \
	size_t l = sizeof(var.tbl.valbuf.colname) - 1; \
	if(strlen(val) < l) { \
		l = strlen(val); } \
	memcpy(var.tbl.valbuf.colname, val, l ); \
	var.tbl.valbuf.colname[l] = 0; \
}

#define SET_COLUMN_TM(var,colname,tval) { \
	if(! var.tbl.colptr.colname ) { \
		var.tbl.colptr.colname = &var.tbl.valbuf.colname; } \
	memcpy(&var.tbl.valbuf.colname, &tval, sizeof(struct tm) ); \
}

#define SET_COLUMN_NULL(var,colname) { \
	if( var.tbl.colptr.colname ) { \
		var.tbl.colptr.colname = 0; } \
}
