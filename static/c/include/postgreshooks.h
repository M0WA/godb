#pragma once

#ifndef _DISABLE_POSTGRES

struct _DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

int postgres_initlib_hook();
int postgres_exitlib_hook();

int postgres_create_hook(struct _DBHandle*);
int postgres_destroy_hook(struct _DBHandle*);

int postgres_connect_hook(struct _DBHandle*);
int postgres_disconnect_hook(struct _DBHandle*);

int postgres_insert_hook(struct _DBHandle*,const struct _InsertStmt *const);
int postgres_update_hook(struct _DBHandle*,const struct _UpdateStmt *const);
int postgres_upsert_hook(struct _DBHandle*,const struct _UpsertStmt *const);
int postgres_delete_hook(struct _DBHandle*,const struct _DeleteStmt *const);
int postgres_select_hook(struct _DBHandle*,const struct _SelectStmt *const,struct _SelectResult*);
int postgres_fetch_hook(struct _DBHandle*,struct _SelectResult*);

#endif
