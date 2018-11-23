#include "where.h"

#include <stdlib.h>

#include "logger.h"
#include "column.h"
#include "helper.h"

static void where_cond_destroy(struct _WhereCondition *c) {
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

	size_t newsize = (clause->cnt + 1) * sizeof(union _WhereStmt*);

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

static int where_comp_op(WhereCompOperator op,char** sql) {
	if(sql || (*sql) == 0) {
		return 1; }
	switch(op) {
	case WHERE_AND:
		return append_string(") AND (", sql);
	case WHERE_OR:
		return append_string(") OR (", sql);
	default:
		return 1;
	}
}

static int where_cond_string(const struct _WhereCondition *cond,const char *specifier,char** sql) {
	if(cond->cnt > 1 && cond->cond != WHERE_EQUAL && cond->cond != WHERE_NOT_EQUAL) {
		LOG_WARN("only equal/not equal allow for range types in where clause");
		return 1; }

	size_t bufsize = cond->def->type == COL_TYPE_STRING ? cond->def->size : 64;
	char *buf = 0;
	if(!specifier) {
		buf = alloca(bufsize);
		if(!buf) {
			return 1; }
	}

	if(append_string("(", sql)) {
		return 1;}
	if(append_string(cond->def->name, sql)) {
		return 1;}
	if(cond->cnt > 1) {
		switch(cond->cond) {
		case WHERE_EQUAL:
			if(append_string(" IN (", sql)) {
				return 1;}
			break;
		case WHERE_NOT_EQUAL:
			if(append_string(" NOT IN (", sql)) {
				return 1;}
			break;
		default:
			LOG_WARN("multi column in where clause cannot combined with 'like' or is null operator");
			return 1;
		}
	} else {
		if(!specifier && get_column_string(buf,bufsize,cond->def,cond->values[0]) ) {
			return 1; }
		switch(cond->cond) {
		case WHERE_EQUAL:
			if(append_string(" = ", sql)) {
				return 1;}
			if(append_string((specifier ? specifier : buf), sql)) {
				return 1;}
			break;
		case WHERE_NOT_EQUAL:
			if(append_string(" != ", sql)) {
				return 1;}
			if(append_string((specifier ? specifier : buf), sql)) {
				return 1;}
			break;
		case WHERE_LIKE:
			if(append_string(" LIKE ", sql)) {
				return 1;}
			if(append_string((specifier ? specifier : buf), sql)) {
				return 1;}
			break;
		case WHERE_NOT_LIKE:
			if(append_string(" NOT LIKE ", sql)) {
				return 1;}
			if(append_string((specifier ? specifier : buf), sql)) {
				return 1;}
			break;
		case WHERE_IS_NULL:
			if(append_string(" IS NULL ", sql)) {
				return 1;}
			break;
		case WHERE_IS_NOT_NULL:
			if(append_string(" IS NOT NULL ", sql)) {
				return 1;}
			break;
		default:
			LOG_WARN("column in where clause cannot combined with 'like' operator");
			return 1;
		}
	}
	for(size_t i = 0; i < cond->cnt; i++) {
		if(!specifier && get_column_string(buf,bufsize,cond->def,cond->values[i]) ) {
			return 1; }
		if(cond->cnt > 1) {
			if(i && append_string(", ", sql)) {
				return 1; }
			if(append_string((specifier ? specifier : buf), sql)) {
				return 1; }
		}
	}
	if(cond->cnt > 1 && append_string(")", sql)) {
		return 1; }
	if(append_string(")", sql)) {
		return 1;}
	return 0;
}

static int where_comp_string(const struct _WhereComposite *comp,const char *specifier,char** sql) {
	for(size_t i = 0; i < comp->cnt; i++) {
		if(i) {
			if( where_comp_op(comp->where[i]->comp,sql) ) {
				return 1; }
		}
		if( where_string(comp->where[i],specifier,sql) ) {
			return 1; }
	}
	return 0;
}

int where_string(const struct _WhereClause *clause,const char *specifier,char** sql) {
	if(clause->cnt == 0) {
		return 0; }
	if(append_string("(", sql)) {
		return 1;}
	for(size_t i = 0; i < clause->cnt; i++) {
		if(i && where_comp_op(clause->comp,sql)) {
			return 1; }

		switch(clause->stmts[i]->cond.type) {
		case WHERE_COND:
			if( where_cond_string(&clause->stmts[i]->cond,specifier,sql) ) {
				LOG_WARN("invalid where condition");
				return 1;
			}
			break;
		case WHERE_COMP:
			for(size_t j = 0; j < clause->stmts[i]->comp.cnt; j++) {
				if( where_comp_string(&clause->stmts[i]->comp,specifier,sql) ) {
					LOG_WARN("invalid where composite");
					return 1;
				}
			}
			break;
		}
	}
	if(append_string(")", sql)) {
			return 1;}
	return 0;
}
