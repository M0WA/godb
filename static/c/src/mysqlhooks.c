#ifndef _DISABLE_MYSQL

#include "mysqlhooks.h"

int mysql_connect_hook(struct _DBHandle* dbh) {
	return 1;
}

int mysql_disconnect_hook(struct _DBHandle* dbh) {
	return 1;
}

int mysql_insert_hook(struct _DBHandle* dbh,struct _InsertStmt* s) {
	return 1;
}

int mysql_update_hook(struct _DBHandle* dbh,struct _UpdateStmt* s) {
	return 1;
}

int mysql_upsert_hook(struct _DBHandle* dbh,struct _UpsertStmt* s) {
	return 1;
}

int mysql_delete_hook(struct _DBHandle* dbh,struct _DeleteStmt* s) {
	return 1;
}

struct _SelectResult* mysql_select_hook(struct _DBHandle* dbh,struct _SelectStmt* s) {
	return 0;
}

#endif
