#include "statements.h"

#include "column.h"
#include "table.h"
#include "helper.h"
#include "where.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int insert_stmt_string(const InsertStmt *const s, ValueSpecifier valspec, char** sql, int skip_autoincrement) {
	char fmt[] = "INSERT INTO %s (%s) VALUES %s";
	char *colnames = 0;
	char *where = 0;
	char *values = 0;
	int rc = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols, skip_autoincrement);
	if(!colnames) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT;	}

	if( insert_values_row_string(s->defs, s->ncols, valspec, s->valbuf, s->nrows, &values, 0, skip_autoincrement) ) {
		rc = 1;
		goto INSERT_STMT_STRING_EXIT;	}

	size_t lenStmt = 1 + (values ? strlen(values) : 0) + strlen(fmt) + strlen(colnames) + strlen(s->defs->table);
	*sql = malloc(lenStmt);
	if(!*sql) {
		return 1; }
	sprintf(*sql,fmt,s->defs->table,colnames,(values ? values : ""));
	LOGF_DEBUG("statement: %s",*sql);

INSERT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	if(where) {
		free(where); }
	if(values) {
		free(values); }
	return rc;
}

int select_stmt_string(const SelectStmt *const s, WhereSpecifier wherespec, char** sql) {
	const char fmt[] = "SELECT %s FROM %s %s %s %s";
	char *colnames = 0;
	char *where = 0;
	char limit[32] = {0};
	int rc = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols,0);
	if(!colnames) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT;	}

	if(where_string(&s->where,wherespec,&where,0)) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }

	get_limit(s->limit, limit);

	size_t wheresize = where ? strlen(where) : 0;
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->defs[0].table) + strlen(colnames) + 1;
	*sql = malloc(sqlsize);
	if(!*sql) {
		rc = 1;
		goto SELECT_STMT_STRING_EXIT; }
	sprintf(*sql,fmt,colnames,s->defs[0].table,(where ? " WHERE " : ""),(where ? where : ""),limit);
	LOGF_DEBUG("statement: %s",*sql);

SELECT_STMT_STRING_EXIT:
	if(colnames) {
		free(colnames); }
	if(where) {
		free(where); }
	return rc;
}

int delete_stmt_string(const DeleteStmt *const s, WhereSpecifier wherespec, char** sql) {
	const char fmt[] = "DELETE FROM %s %s %s";
	char *where = 0;
	int rc = 0;

	if(where_string(&s->where,wherespec,&where,0)) {
		rc = 1;
		goto DELETE_STMT_STRING_EXIT; }

	size_t wheresize = where ? strlen(where) : 0;
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + strlen(s->def->name) + 1;
	*sql = malloc(sqlsize);
	if(!*sql) {
		goto DELETE_STMT_STRING_EXIT;
		rc = 1; }
	sprintf(*sql,fmt,s->def->name,(where ? " WHERE " : ""),(where ? where : ""));
	LOGF_DEBUG("statement: %s",*sql);

DELETE_STMT_STRING_EXIT:
	if(where) {
		free(where); }
	return rc;
}

int update_stmt_string(const UpdateStmt *const s, ValueSpecifier valspec, WhereSpecifier wherespec, char** sql, int skip_autoincrement) {
	const char fmt[] = "UPDATE %s SET %s %s %s";
	char *where = 0;
	char *values = 0;
	int rc = 0;
	size_t serial = 1;

	if( update_values_string(s->defs, s->ncols, valspec, s->valbuf, &values, &serial, skip_autoincrement) ) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT;	}

	if(where_string(&s->where,wherespec,&where,&serial)) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT; }

	size_t wheresize = where ? (strlen(where) + strlen(" WHERE ")) : 0;
	size_t valuesize = values ? strlen(values) : 0;
	size_t sqlsize = strlen(fmt) + wheresize + valuesize + strlen(s->defs[0].table) + 1;
	*sql = malloc(sqlsize);
	if(!*sql) {
		rc = 1;
		goto UPDATE_STMT_STRING_EXIT; }
	sprintf(*sql,fmt,s->defs[0].table,(values ? values : ""),(where ? " WHERE " : ""),(where ? where : ""));
	LOGF_DEBUG("statement: %s",*sql);

UPDATE_STMT_STRING_EXIT:
	if(where) {
		free(where); }
	if(values) {
		free(values); }
	return rc;
}
