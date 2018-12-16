#include "where.h"

#include <stdlib.h>

#include "logger.h"
#include "column.h"
#include "helper.h"
#include "stringbuf.h"

static void where_cond_destroy(struct _WhereCondition *c) {
}

static void where_comp_destroy(struct _WhereComposite *comp, WhereConditionFreeFunc cond) {
	if(!comp) {
		return; }

	if(comp->where) {
		for(size_t i = 0; i < comp->cnt; i++) {
			where_destroy_free(comp->where[i],cond);
		}
		free(comp->where);
		comp->where = 0;
	}
}

static void where_stmt_destroy(union _WhereStmt *s, WhereConditionFreeFunc cond) {
	if(!s) {
		return;	}

	switch(s->cond.type) {
	case WHERE_COND:
		cond(&s->cond);
		break;
	case WHERE_COMP:
		where_comp_destroy(&s->comp,cond);
		break;
	default:
		return;
	}
}

void where_destroy_free(struct _WhereClause *clause, WhereConditionFreeFunc cond) {
	if(!clause||!clause->cnt||!clause->stmts) {
		return;	}
	for(size_t i = 0; i < clause->cnt; i++) {
		where_stmt_destroy(clause->stmts[i],cond);
	}
	free(clause->stmts);
	clause->stmts = 0;
}

void where_destroy(struct _WhereClause *clause) {
	where_destroy_free(clause,where_cond_destroy);
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

static int where_comp_op(WhereCompOperator op,struct _StringBuf *sql) {
	if(!sql) {
		return 1; }
	switch(op) {
	case WHERE_AND:
		return stringbuf_append(sql,") AND (");
	case WHERE_OR:
		return stringbuf_append(sql,") OR (");
	default:
		return 1;
	}
}

static int where_cond_string(const struct _WhereCondition *cond,WhereSpecifier spec,struct _StringBuf *sql, size_t *serial) {
	if(cond->cnt > 1 && cond->cond != WHERE_EQUAL && cond->cond != WHERE_NOT_EQUAL) {
		LOG_WARN("only equal/not equal allow for range types in where clause");
		return 1; }

	if(stringbuf_append(sql,"(")) {
		return 1;}
	if(stringbuf_append(sql,cond->def->name)) {
		return 1;}
	if(cond->cnt > 1) {
		switch(cond->cond) {
		case WHERE_EQUAL:
			if(stringbuf_append(sql," IN (")) {
				return 1;}
			break;
		case WHERE_NOT_EQUAL:
			if(stringbuf_append(sql," NOT IN (")) {
				return 1;}
			break;
		default:
			LOG_WARN("multi column in where clause cannot combined with 'like' or is null operator");
			return 1;
		}
	} else {
		switch(cond->cond) {
		case WHERE_EQUAL:
			if(stringbuf_append(sql," = ")) {
				return 1;}
			if( spec(cond->def,cond->values[0],sql,serial) ) {
				return 1;}
			(*serial)++;
			break;
		case WHERE_NOT_EQUAL:
			if(stringbuf_append(sql," != ")) {
				return 1;}
			if( spec(cond->def,cond->values[0],sql,serial) ) {
				return 1;}
			(*serial)++;
			break;
		case WHERE_LIKE:
			if(stringbuf_append(sql," LIKE ")) {
				return 1;}
			if( spec(cond->def,cond->values[0],sql,serial) ) {
				return 1;}
			(*serial)++;
			break;
		case WHERE_NOT_LIKE:
			if(stringbuf_append(sql," NOT LIKE ")) {
				return 1;}
			if( spec(cond->def,cond->values[0],sql,serial) ) {
				return 1;}
			(*serial)++;
			break;
		case WHERE_IS_NULL:
			if(stringbuf_append(sql," IS NULL ")) {
				return 1;}
			break;
		case WHERE_IS_NOT_NULL:
			if(stringbuf_append(sql," IS NOT NULL ")) {
				return 1;}
			break;
		default:
			LOG_WARN("column in where clause cannot combined with 'like' operator");
			return 1;
		}
	}
	if(cond->cnt > 1) {
		for(size_t i = 0; i < cond->cnt; i++) {
			if(i && stringbuf_append(sql,",")) {
				return 1; }
			if( spec(cond->def,cond->values[i],sql,serial) ) {
				return 1;}
			(*serial)++;
		}
		if(stringbuf_append(sql,")")) {
			return 1; }
	}
	if(stringbuf_append(sql,")")) {
		return 1;}
	return 0;
}

static int where_comp_string(const struct _WhereComposite *comp,WhereSpecifier spec,struct _StringBuf *sql, size_t *serial) {
	for(size_t i = 0; i < comp->cnt; i++) {
		if(i) {
			if( where_comp_op(comp->where[i]->comp,sql) ) {
				return 1; }
		}
		if( where_string(comp->where[i],spec,sql,serial) ) {
			return 1; }
	}
	return 0;
}

int where_string(const struct _WhereClause *clause,WhereSpecifier spec,struct _StringBuf *sql, size_t *serial) {
	size_t tmpserial = 1;
	size_t *realserial = serial ? serial : &tmpserial;
	if(clause->cnt == 0) {
		return 0; }
	if(stringbuf_append(sql,"(")) {
		return 1;}
	for(size_t i = 0; i < clause->cnt; i++) {
		if(i && where_comp_op(clause->comp,sql)) {
			return 1; }

		switch(clause->stmts[i]->cond.type) {
		case WHERE_COND:
			if( where_cond_string(&clause->stmts[i]->cond,spec,sql,realserial) ) {
				LOG_WARN("invalid where condition");
				return 1;
			}
			break;
		case WHERE_COMP:
			for(size_t j = 0; j < clause->stmts[i]->comp.cnt; j++) {
				if( where_comp_string(&clause->stmts[i]->comp,spec,sql,realserial) ) {
					LOG_WARN("invalid where composite");
					return 1;
				}
			}
			break;
		}
	}
	if(stringbuf_append(sql,")")) {
			return 1;}
	return 0;
}

int where_generic_value_specifier(const struct _DBColumnDef *def,const void *value,struct _StringBuf *sql,size_t* serial) {
	size_t bufsize = def->type == COL_TYPE_STRING ? def->size : 64;
	char *buf = alloca(bufsize);
	if(!buf) {
		return 1; }
	if(get_column_string(buf,bufsize,def,value)) {
		return 1; }
	if(stringbuf_append(sql,buf)) {
		return 1; }
	return 0;
}
