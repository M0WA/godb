#pragma once

#ifndef _DISABLE_POSTGRES

struct _DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

int postgres_connect_hook(struct _DBHandle*);
int postgres_disconnect_hook(struct _DBHandle*);
int postgres_insert_hook(struct _DBHandle*,struct _InsertStmt*);
int postgres_update_hook(struct _DBHandle*,struct _UpdateStmt*);
int postgres_upsert_hook(struct _DBHandle*,struct _UpsertStmt*);
int postgres_delete_hook(struct _DBHandle*,struct _DeleteStmt*);
struct _SelectResult* postgres_select_hook(struct _DBHandle*,struct _SelectStmt*);

#endif
