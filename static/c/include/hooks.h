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

typedef int (*DBConnectHook)(struct _DBHandle* h);
typedef int (*DBDisconnectHook)(struct _DBHandle* h);

struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

typedef int (*DBInsertHook)(struct _DBHandle*,struct _InsertStmt*);
typedef int (*DBUpdateHook)(struct _DBHandle*,struct _UpdateStmt*);
typedef int (*DBUpsertHook)(struct _DBHandle*,struct _UpsertStmt*);
typedef int (*DBDeleteHook)(struct _DBHandle*,struct _DeleteStmt*);
typedef struct _SelectResult* (*DBSelectHook)(struct _DBHandle*,struct _SelectStmt*);

typedef struct _DBHooks {
	DBConnectHook connect;
	DBDisconnectHook disconnect;

	DBInsertHook insert;
	DBUpdateHook update;
	DBUpsertHook upsert;
	DBDeleteHook delete;
	DBSelectHook select;
} DBHooks;
