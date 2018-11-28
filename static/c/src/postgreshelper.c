#ifdef _ENABLE_POSTGRESHELPER

#include "postgreshelper.h"

#include "where.h"
#include "column.h"
#include "helper.h"
#include "logger.h"
#include "statements.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alloca.h>

int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial) {
	size_t bufsize = def->type == COL_TYPE_STRING ? def->size : 64;
	char *buf = alloca(bufsize);
	if(!buf) {
		return 1; }
	if(get_column_string(buf,bufsize,def,value)) {
		return 1; }
	if(append_string(buf,sql)) {
		return 1; }
	return 0;
}

int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial) {

	//https://www.postgresql.org/docs/9.4/libpq-exec.html -> PQexecParams

	size_t bufsize = def->type == COL_TYPE_STRING ? def->size : 64;
	char *buf = alloca(bufsize);
	if(!buf) {
		return 1; }
	if(get_column_string(buf,bufsize,def,value)) {
		return 1; }
	if(append_string(buf,sql)) {
		return 1; }
	return 0;
}

int postgres_time_to_tm(const char *val, struct tm *t) {
	memset(t, 0, sizeof(struct tm));
	if( !strptime(val, "%Y-%m-%d %H:%M:%S", t) ) {
		LOGF_WARN("invalid postgres date: %s",val);
		return 1;
	}
	return 0;
}

int postgres_upsert_stmt_string(const UpsertStmt *const s, char** sql) {
	//example:
	/*
	WITH new_values
	  (domain)
	AS
	  (VALUES ('domainname') ),
	upsert AS
	(
	  UPDATE deepnet.public.secondleveldomains m
	  SET
		domain = nv.domain
	  FROM new_values nv
	  WHERE
		m.domain = nv.domain
	  RETURNING m.*
	)
	INSERT INTO deepnet.public.secondleveldomains (domain)
	SELECT
	  domain
	FROM new_values
	WHERE NOT EXISTS
	(
	  SELECT
		1
	  FROM
		upsert up
	  WHERE
		domain = new_values.domain
	)
	*/
	return 1;
}

#endif
