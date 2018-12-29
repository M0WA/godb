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

int insert_stmt_string(const InsertStmt *const s, ValueSpecifier valspec, struct _StringBuf *sql) {
	char fmt[] = "INSERT INTO %s (%s) VALUES %s";
	char *colnames = 0;
	int rc = 0;

	StringBuf values;
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);

	colnames = comma_concat_colnames(s->dbtbl->def->cols,s->dbtbl->def->ncols);
	if(!colnames) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT;	}

	if( insert_values_row_string(s->dbtbl, valspec, &values, 0) ) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT; }

	size_t sqlsize = 1 + stringbuf_strlen(&values) + strlen(fmt) + strlen(colnames) + strlen(s->dbtbl->def->name);
	if(stringbuf_resize(sql,sqlsize)) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT; }
	snprintf(stringbuf_buf(sql),sqlsize,fmt,s->dbtbl->def->name,colnames,(stringbuf_get(&values) ? stringbuf_get(&values) : ""));
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

INSERT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	stringbuf_destroy(&values);
	return rc;
}

int select_stmt_string(const SelectStmt *const s, WhereSpecifier wherespec, struct _StringBuf *sql) {
	const char fmt[] = "SELECT %s FROM %s %s %s %s";
	char *colnames = 0;
	char limit[32] = {0};
	int rc = 0;

	StringBuf where;
	stringbuf_init(&where,SQL_VALUE_ALLOC_BLOCK);

	colnames = comma_concat_colnames(s->def->cols,s->def->ncols);
	if(!colnames) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT;	}

	if(where_string(&s->where,wherespec,&where,0)) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }

	get_limit(s->limit, limit);

	size_t wheresize = stringbuf_strlen(&where);
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->def->name) + strlen(colnames) + 1;
	if(stringbuf_resize(sql,sqlsize)) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }
	snprintf(stringbuf_buf(sql),sqlsize,fmt,
			colnames,
			s->def->name,
			(stringbuf_get(&where) ? " WHERE " : ""),
			(stringbuf_get(&where) ? stringbuf_get(&where) : ""),
			limit
	);
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

SELECT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	stringbuf_destroy(&where);
	return rc;
}

int delete_stmt_string(const DeleteStmt *const s, WhereSpecifier wherespec, struct _StringBuf *sql) {
	const char fmt[] = "DELETE FROM %s %s %s";
	int rc = 0;

	StringBuf where;
	stringbuf_init(&where,SQL_VALUE_ALLOC_BLOCK);

	if(where_string(&s->where,wherespec,&where,0)) {
		rc = 1;
		goto DELETE_STMT_STRING_EXIT; }

	size_t wheresize = stringbuf_strlen(&where);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + strlen(s->def->name) + 1;
	if(stringbuf_resize(sql,sqlsize)) {
		rc = 1;
		goto DELETE_STMT_STRING_EXIT; }
	snprintf(stringbuf_buf(sql),sqlsize,fmt,s->def->name,( stringbuf_get(&where) ? " WHERE " : ""),( stringbuf_get(&where) ?  stringbuf_get(&where) : ""));
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

DELETE_STMT_STRING_EXIT:
	stringbuf_destroy(&where);
	return rc;
}

int update_stmt_string(const UpdateStmt *const s, ValueSpecifier valspec, WhereSpecifier wherespec, struct _StringBuf *sql) {
	const char fmt[] = "UPDATE %s SET %s %s %s";
	int rc = 0;
	size_t serial = 1;

	StringBuf where,values;
	stringbuf_init(&where,SQL_VALUE_ALLOC_BLOCK);
	stringbuf_init(&values,SQL_VALUE_ALLOC_BLOCK);

	if( update_values_string(s->dbtbl, valspec, &values, &serial) ) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT;	}

	if(where_string(&s->where,wherespec,&where,&serial)) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT; }

	size_t wheresize = stringbuf_strlen(&where);
	size_t valuesize = stringbuf_strlen(&values);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + valuesize + strlen(s->dbtbl->def->name) + 1;
	if(stringbuf_resize(sql,sqlsize)) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT; }
	snprintf(stringbuf_buf(sql),sqlsize,fmt,s->dbtbl->def->name,
			(stringbuf_get(&values) ? stringbuf_get(&values) : ""),
			(stringbuf_get(&where) ? " WHERE " : ""),(stringbuf_get(&where) ? stringbuf_get(&where) : ""));
	LOGF_DEBUG("statement: %s",stringbuf_get(sql));

UPDATE_STMT_STRING_EXIT:
	stringbuf_destroy(&where);
	stringbuf_destroy(&values);
	return rc;
}
