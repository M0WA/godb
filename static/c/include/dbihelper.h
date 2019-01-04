#ifndef _DISABLE_DBI

struct _DBHandle;
struct _UpsertStmt;
struct _StringBuf;

int dbi_upsert_stmt_string(const struct _DBHandle *dbh,const struct _UpsertStmt *const s, const char *delimiter, struct _StringBuf*);

#endif
