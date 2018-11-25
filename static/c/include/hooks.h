#pragma once

#define REGISTER_HOOKS(dbhandle,dbtype) ({ \
	dbhandle->hooks.initlib = &dbtype##_initlib_hook; \
	dbhandle->hooks.exitlib = &dbtype##_exitlib_hook; \
	dbhandle->hooks.create  = &dbtype##_create_hook; \
	dbhandle->hooks.destroy = &dbtype##_destroy_hook; \
	dbhandle->hooks.connect = &dbtype##_connect_hook; \
	dbhandle->hooks.disconnect = &dbtype##_disconnect_hook; \
	dbhandle->hooks.insert = &dbtype##_insert_hook; \
	dbhandle->hooks.update = &dbtype##_update_hook; \
	dbhandle->hooks.upsert = &dbtype##_upsert_hook; \
	dbhandle->hooks.delete = &dbtype##_delete_hook; \
	dbhandle->hooks.select = &dbtype##_select_hook; \
	dbhandle->hooks.fetch  = &dbtype##_fetch_hook; \
});

struct _DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

typedef int (*DBInitLibHook)();
typedef int (*DBExitLibHook)();

typedef int (*DBCreateHandleHook)(struct _DBHandle*);
typedef int (*DBDestroyHandleHook)(struct _DBHandle*);

typedef int (*DBConnectHook)(struct _DBHandle*);
typedef int (*DBDisconnectHook)(struct _DBHandle*);

typedef int (*DBInsertHook)(struct _DBHandle*,const struct _InsertStmt *const);
typedef int (*DBUpdateHook)(struct _DBHandle*,const struct _UpdateStmt *const);
typedef int (*DBUpsertHook)(struct _DBHandle*,const struct _UpsertStmt *const);
typedef int (*DBDeleteHook)(struct _DBHandle*,const struct _DeleteStmt *const);
typedef int (*DBSelectHook)(struct _DBHandle*,const struct _SelectStmt *const,struct _SelectResult*);
typedef int (*DBFetchRowHook)(struct _DBHandle*,struct _SelectResult*);

typedef struct _DBHooks {
	DBInitLibHook initlib;
	DBExitLibHook exitlib;

	DBCreateHandleHook  create;
	DBDestroyHandleHook destroy;

	DBConnectHook    connect;
	DBDisconnectHook disconnect;

	DBInsertHook   insert;
	DBUpdateHook   update;
	DBUpsertHook   upsert;
	DBDeleteHook   delete;
	DBSelectHook   select;
	DBFetchRowHook fetch;
} DBHooks;
