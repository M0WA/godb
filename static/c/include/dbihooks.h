#pragma once

#ifndef _DISABLE_DBI

struct _DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

int dbi_connect_hook(struct _DBHandle*);
int dbi_disconnect_hook(struct _DBHandle*);
int dbi_insert_hook(struct _DBHandle*,const struct _InsertStmt *const);
int dbi_update_hook(struct _DBHandle*,const struct _UpdateStmt *const);
int dbi_upsert_hook(struct _DBHandle*,const struct _UpsertStmt *const);
int dbi_delete_hook(struct _DBHandle*,const struct _DeleteStmt *const);
int dbi_select_hook(struct _DBHandle*,const struct _SelectStmt *const,struct _SelectResult*);
int dbi_fetch_hook(struct _DBHandle*,struct _SelectResult*);

#endif
