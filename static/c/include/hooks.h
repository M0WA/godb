#pragma once

#define REGISTER_HOOKS(dbhandle,dbtype) ({ \
	dbhandle->hooks.connect = &dbtype##_connect_hook; \
	dbhandle->hooks.disconnect = &dbtype##_disconnect_hook; \
	dbhandle->hooks.insert = &dbtype##_insert_hook; \
	dbhandle->hooks.update = &dbtype##_update_hook; \
	dbhandle->hooks.upsert = &dbtype##_upsert_hook; \
	dbhandle->hooks.delete = &dbtype##_delete_hook; \
	dbhandle->hooks.select = &dbtype##_select_hook; \
});

struct _DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

typedef int (*DBConnectHook)(struct _DBHandle* h);
typedef int (*DBDisconnectHook)(struct _DBHandle* h);

typedef int (*DBInsertHook)(struct _DBHandle*,const struct _InsertStmt *const);
typedef int (*DBUpdateHook)(struct _DBHandle*,const struct _UpdateStmt *const);
typedef int (*DBUpsertHook)(struct _DBHandle*,const struct _UpsertStmt *const);
typedef int (*DBDeleteHook)(struct _DBHandle*,const struct _DeleteStmt *const);
typedef int (*DBSelectHook)(struct _DBHandle*,const struct _SelectStmt *const,struct _SelectResult** res);

typedef struct _DBHooks {
	DBConnectHook    connect;
	DBDisconnectHook disconnect;

	DBInsertHook insert;
	DBUpdateHook update;
	DBUpsertHook upsert;
	DBDeleteHook delete;
	DBSelectHook select;
} DBHooks;
