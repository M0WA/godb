#ifndef _DISABLE_POSTGRES

#include "postgreshooks.h"

int postgres_connect_hook(struct _DBHandle* dbh) {
	return 1;
}

int postgres_disconnect_hook(struct _DBHandle* dbh) {
	return 1;
}

int postgres_insert_hook(struct _DBHandle* dbh,struct _InsertStmt* s) {
	return 1;
}

int postgres_update_hook(struct _DBHandle* dbh,struct _UpdateStmt* s) {
	return 1;
}

int postgres_upsert_hook(struct _DBHandle* dbh,struct _UpsertStmt* s) {
	return 1;
}

int postgres_delete_hook(struct _DBHandle* dbh,struct _DeleteStmt* s) {
	return 1;
}

struct _SelectResult* postgres_select_hook(struct _DBHandle* dbh,struct _SelectStmt* s) {
	return 0;
}

#endif
