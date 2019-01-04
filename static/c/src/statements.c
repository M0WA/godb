#include "statements.h"

#include "column.h"
#include "table.h"
#include "helper.h"
#include "where.h"
#include "logger.h"
#include "stringbuf.h"
#include "dblimits.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int create_insert_stmt(struct _InsertStmt *s,const struct _DBTable *dbtbl) {
	memset(s,0,sizeof(struct _InsertStmt));
	s->dbtbl = dbtbl;
	return 0;
}

int create_select_stmt(struct _SelectStmt *s,const struct _DBTableDef *def) {
	memset(s,0,sizeof(struct _SelectStmt));
	s->def = def;
	return 0;
}

int create_delete_stmt(struct _DeleteStmt *s,const struct _DBTableDef *def) {
	memset(s,0,sizeof(struct _DeleteStmt));
	s->def = def;
	return 0;
}

int create_upsert_stmt(struct _UpsertStmt *s,const struct _DBTable *dbtbl) {
	memset(s,0,sizeof(struct _UpsertStmt));
	s->dbtbl = dbtbl;
	return 0;
}

int create_update_stmt(struct _UpdateStmt *s,const struct _DBTable *dbtbl) {
	memset(s,0,sizeof(struct _UpdateStmt));
	s->dbtbl = dbtbl;
	return 0;
}

int insert_stmt_string(const InsertStmt *const s, ValueSpecifier valspec, const char *delimiter, struct _StringBuf *sql) {
	char fmt[] = "INSERT INTO %s (%s) VALUES %s";
	int rc = 0;

	StringBuf values, colnames;
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&colnames,SQL_VALUE_ALLOC_BLOCK);

	if(comma_concat_colnames_insert(&colnames,s->dbtbl->def->cols,s->dbtbl->def->ncols,delimiter)) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT;	}

	if( insert_values_row_string(s->dbtbl, valspec, &values, 0) ) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT; }

	size_t sqlsize = 1 + stringbuf_strlen(&values) + strlen(fmt) + stringbuf_strlen(&colnames) + strlen(s->dbtbl->def->name);
	if(stringbuf_resize(sql,sqlsize)) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT; }
	snprintf(stringbuf_buf(sql),sqlsize,fmt,s->dbtbl->def->name,stringbuf_get(&colnames),(stringbuf_get(&values) ? stringbuf_get(&values) : ""));
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

INSERT_STMT_STRING_EXIT:
	stringbuf_destroy(&values);
	stringbuf_destroy(&colnames);
	return rc;
}

int select_stmt_string(const SelectStmt *const s, WhereSpecifier wherespec, const char *delimiter, struct _StringBuf *sql) {
	const char *del = delimiter ? delimiter : "";
	const char fmt[] = "SELECT %s FROM %s %s %s %s %s";
	char limit[32] = {0};
	int rc = 0;

	StringBuf where,join,colnames;
	stringbuf_init(&where,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&join,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&colnames,SQL_VALUE_ALLOC_BLOCK);

	if(comma_concat_colnames_select(&colnames,s->def->cols,s->def->ncols,del)) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }

	if(where_string(&s->where,wherespec,delimiter,&where,0)) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }

	if(join_clause_string(&s->joins,delimiter,&join)) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }

	get_limit(s->limit, limit);

	if( stringbuf_appendf(sql,fmt,
			stringbuf_get(&colnames), s->def->name, s->def->name,
			(stringbuf_get(&join) ? stringbuf_get(&join) : ""),
			(stringbuf_get(&where) ? " WHERE " : ""),
			(stringbuf_get(&where) ? stringbuf_get(&where) : ""),
			limit) ) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT;
	}
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

SELECT_STMT_STRING_EXIT:
	stringbuf_destroy(&colnames);
	stringbuf_destroy(&where);
	stringbuf_destroy(&join);
	return rc;
}

int delete_stmt_string(const DeleteStmt *const s, WhereSpecifier wherespec, const char *delimiter, struct _StringBuf *sql) {
	const char fmt[] = "DELETE FROM %s %s %s";
	int rc = 0;

	StringBuf where;
	stringbuf_init(&where,SQL_VALUE_ALLOC_BLOCK);

	if(where_string(&s->where,wherespec,delimiter,&where,0)) {
		rc = 1;
		goto DELETE_STMT_STRING_EXIT; }

	if(stringbuf_appendf(sql,fmt,
			s->def->name,
			( stringbuf_get(&where) ? " WHERE " : ""),
			( stringbuf_get(&where) ?  stringbuf_get(&where) : "")) ) {
		rc = 1;
		goto DELETE_STMT_STRING_EXIT; }
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

DELETE_STMT_STRING_EXIT:
	stringbuf_destroy(&where);
	return rc;
}

int update_stmt_string(const UpdateStmt *const s, ValueSpecifier valspec, WhereSpecifier wherespec, const char *delimiter, struct _StringBuf *sql) {
	const char fmt[] = "UPDATE %s SET %s %s %s";
	int rc = 0;
	size_t serial = 1;

	StringBuf where,values;
	stringbuf_init(&where,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);

	if( update_values_string(s->dbtbl, valspec, delimiter, &values, &serial) ) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT;	}

	if(where_string(&s->where,wherespec,delimiter,&where,&serial)) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT; }

	if(stringbuf_appendf(sql,fmt,s->dbtbl->def->name,
			(stringbuf_get(&values) ? stringbuf_get(&values) : ""),
			(stringbuf_get(&where) ? " WHERE " : ""),
			(stringbuf_get(&where) ? stringbuf_get(&where) : "") )) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT; }
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

UPDATE_STMT_STRING_EXIT:
	stringbuf_destroy(&where);
	stringbuf_destroy(&values);
	return rc;
}

void destroy_select_stmt(struct _SelectStmt *s) {
	where_destroy(&s->where);
	join_destroy(&s->joins);
}

void destroy_delete_stmt(struct _DeleteStmt *s) {
	where_destroy(&s->where);
}

void destroy_update_stmt(struct _UpdateStmt *s) {
	where_destroy(&s->where);
	join_destroy(&s->joins);
}
