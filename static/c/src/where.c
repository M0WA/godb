#include "where.h"

#include <stdlib.h>

static void where_cond_destroy(struct _WhereCondition *c) {
	/*
	if(c) {
		free(c);
	}
	*/
}

static void where_comp_destroy(struct _WhereComposite *comp) {
	if(!comp) {
		return; }

	if(comp->where) {
		for(size_t i = 0; i < comp->cnt; i++) {
			where_destroy(comp->where[i]);
		}
		free(comp->where);
		comp->where = 0;
	}
}

static void where_stmt_destroy(union _WhereStmt *s) {
	if(!s) {
		return;	}

	switch(s->cond.type) {
	case WHERE_COND:
		where_cond_destroy(&s->cond);
		break;
	case WHERE_COMP:
		where_comp_destroy(&s->comp);
		break;
	default:
		return;
	}
}

void where_destroy(struct _WhereClause *clause) {
	if(!clause||!clause->cnt||!clause->stmts) {
		return;	}
	for(size_t i = 0; i < clause->cnt; i++) {
		where_stmt_destroy(clause->stmts[i]);
	}
	free(clause->stmts);
	clause->stmts = 0;
}

int where_comp_append(struct _WhereComposite *comp,struct _WhereClause *clause) {
	if(!comp || !clause) {
		return 1; }

	size_t newsize = (comp->cnt + 1) + sizeof(struct _WhereClause*);
	if(!comp->where) {
		comp->where = malloc(newsize);
		if(!comp->where) {
			return 1; }
	} else {
		struct _WhereClause** tmp = realloc(comp->where,newsize);
		if(!tmp) {
			return 1; }
		comp->where = tmp;
	}

	comp->where[comp->cnt] = clause;
	comp->cnt++;
	return 0;
}

int where_append(struct _WhereClause *clause,union _WhereStmt *stmt) {
	if(!clause || !stmt ) {
		return 1; }

	size_t newsize = (clause->cnt + 1) + sizeof(union _WhereStmt*);

	if(!clause->stmts) {
		clause->stmts = malloc(newsize);
		if(!clause->stmts) {
			return 1; }
	} else {
		union _WhereStmt**tmp = realloc(clause->stmts,newsize);
		if(!tmp) {
			return 1; }
		clause->stmts = tmp;
	}

	clause->stmts[clause->cnt] = stmt;
	clause->cnt++;
	return 0;
}
