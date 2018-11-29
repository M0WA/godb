#ifndef _DISABLE_DBI

struct _DBHandle;
struct _UpsertStmt;

int dbi_upsert_stmt_string(const struct _DBHandle *dbh,const struct _UpsertStmt *const s, char** sql);

#endif
