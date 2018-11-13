#pragma once

#ifndef _DISABLE_MYSQL

struct _DBHandle;
struct _InsertStmt;
struct _UpdateStmt;
struct _UpsertStmt;
struct _DeleteStmt;
struct _SelectStmt;
struct _SelectResult;

int mysql_connect_hook(struct _DBHandle*);
int mysql_disconnect_hook(struct _DBHandle*);
int mysql_insert_hook(struct _DBHandle*,struct _InsertStmt*);
int mysql_update_hook(struct _DBHandle*,struct _UpdateStmt*);
int mysql_upsert_hook(struct _DBHandle*,struct _UpsertStmt*);
int mysql_delete_hook(struct _DBHandle*,struct _DeleteStmt*);
struct _SelectResult* mysql_select_hook(struct _DBHandle*,struct _SelectStmt*);

#endif
