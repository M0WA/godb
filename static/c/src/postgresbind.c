#ifndef _DISABLE_POSTGRES

#include <stdlib.h>
#include <string.h>

#include "postgresbind.h"
#include "column.h"
#include "logger.h"
#include "where.h"
#include "helper.h"

static int postgres_where_comp(const struct _WhereComposite *comp, PostgresParamWrapper *param) {
	for(size_t i = 0; i < comp->cnt; i++) {
		if( postgres_where(comp->where[i],param) ) {
			return 1; }
	}
	return 1;
}

static int postgres_where_cond(const struct _WhereCondition *cond, PostgresParamWrapper *param) {
	if(cond->cnt > 1 && cond->cond != WHERE_EQUAL && cond->cond != WHERE_NOT_EQUAL) {
		LOG_WARN("only equal/not equal allow for range types in where clause");
		return 1; }

	unsigned long buffer_length = get_column_bufsize(cond->def);
	for(size_t i = 0,pos = 0; i < cond->cnt; i++, pos += buffer_length) {
		if( postgres_param_append(cond->def,cond->values[i],param) ) {
			return 1; }
	}
	return 0;
}

int postgres_where(const struct _WhereClause *clause,PostgresParamWrapper *param) {
	if(clause->cnt == 0) {
		return 0; }
	for(size_t i = 0; i < clause->cnt; i++) {
		switch(clause->stmts[i]->cond.type) {
		case WHERE_COND:
			if( postgres_where_cond(&clause->stmts[i]->cond, param) ) {
				LOG_WARN("invalid condition for postgres");
				return 1;
			}
			break;
		case WHERE_COMP:
			for(size_t j = 0; j < clause->stmts[i]->comp.cnt; j++) {
				if( postgres_where_comp(&clause->stmts[i]->comp, param) ) {
					LOG_WARN("invalid composite for postgres");
					return 1;
				}
			}
			break;
		}
	}
	return 0;
}

int postgres_param_append(const struct _DBColumnDef *col,const void *const val,PostgresParamWrapper *param) {
	switch(col->type) {
	case COL_TYPE_STRING:
		//param->types[param->nparam] = 25; //TEXTOID
		param->formats[param->nparam] = 0;
		param->lengths[param->nparam] = strlen((const char*)val);
		param->values[param->nparam] = val;
		break;
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= sizeof(short)) {
			//param->types[param->nparam] = 21; //INT2OID
		} else if(col->size <= sizeof(long) || col->size == 0) {
			//param->types[param->nparam] = 23; //INT4OID
		} else if (col->size <= sizeof(long long)) {
			//param->types[param->nparam] = 20; //INT8OID
		} else {
			LOGF_WARN("invalid int size: %lu",col->size);
			return 1;
		}
		param->formats[param->nparam] = 0;
		if( get_column_string(param->buf[param->nparam],POSTGRES_BIND_BUF,col,val) ) {
			LOG_WARN("cannot convert to int");
			return 1;
		}
		param->values[param->nparam] = param->buf[param->nparam];
		param->lengths[param->nparam] = strlen((const char*)param->buf[param->nparam]);
		break;
	case COL_TYPE_FLOAT:
		//param->types[param->nparam] = 701; //FLOAT8OID
		param->formats[param->nparam] = 0;
		if( get_column_string(param->buf[param->nparam],POSTGRES_BIND_BUF,col,val) ) {
			LOG_WARN("cannot convert to float");
			return 1;
		}
		param->lengths[param->nparam] = strlen((const char*)param->buf[param->nparam]);
		param->values[param->nparam] = param->buf[param->nparam];
		break;
	case COL_TYPE_DATETIME:
		//param->types[param->nparam] = 1114; //TIMESTAMPOID;
		param->formats[param->nparam] = 0;
		if( get_column_string(param->buf[param->nparam],POSTGRES_BIND_BUF,col,val) ) {
			LOG_WARN("cannot convert to datetime");
			return 1;
		}
		param->lengths[param->nparam] = strlen((const char*)param->buf[param->nparam]);
		param->values[param->nparam] = param->buf[param->nparam];
		break;
	default:
		return 1;
	}

	param->nparam++;
	return 0;
}

int postgres_values(const struct _DBColumnDef *defs,size_t ncols,const void *const*const values,PostgresParamWrapper *param) {
	for(size_t i = 0; i < ncols; i++) {
		if(defs[i].autoincrement) {
			continue; }
		if( postgres_param_append(&(defs[i]),values[i],param) ) {
			return 1;
		}
	}
	return 0;
}

int postgres_where_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t* serial) {
	char buf[255] = {0};
	snprintf(buf,255,"$%zu",*serial);
	if(append_string(buf,sql)) {
		return 1; }
	return 0;
}

int postgres_values_specifier(const struct _DBColumnDef *def,const void *value,char** sql,size_t *serial) {
	//https://www.postgresql.org/docs/9.4/libpq-exec.html -> PQexecParams
	char buf[255] = {0};
	snprintf(buf,255,"$%zu",*serial);
	if(append_string(buf,sql)) {
		return 1; }
	return 0;
}

#endif
