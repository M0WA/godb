#ifndef _DISABLE_DBI

#include "dbihooks.h"
#include "dbhandle_impl.h"
#include "logger.h"
#include "helper.h"
#include "column.h"
#include "statements.h"
#include "selectresult.h"

#include <string.h>

int dbi_connect_hook(struct _DBHandle *dbh) {
    dbi_conn_set_option(dbh->dbi.conn, "host", dbh->config.host /* dbh->config.port */);
    dbi_conn_set_option(dbh->dbi.conn, "username", dbh->config.user);
    dbi_conn_set_option(dbh->dbi.conn, "password", dbh->config.pass);
    dbi_conn_set_option(dbh->dbi.conn, "dbname", dbh->config.name);
    dbi_conn_set_option(dbh->dbi.conn, "encoding", "UTF-8");

    int connerr = dbi_conn_connect(dbh->dbi.conn);
	if(connerr != 0) {
		LOGF_WARN("dbi_conn_connect() failed: %d",connerr);
		return 1;}
	return 0;
}

int dbi_disconnect_hook(struct _DBHandle *dbh) {
	if(!dbh) {
		return 0; }
	dbi_conn_close(dbh->dbi.conn);
    dbi_shutdown_r(dbh->dbi.inst);
	return 0;
}

int dbi_insert_hook(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	int rc = 0;
	char *colnames = 0;
	char *values = 0;
	char *stmtbuf = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto DBI_INSERT_EXIT; }

	void **colbuf = alloca(sizeof(void*) * s->ncols);
	if(!colbuf) {
		return 1; }
	for(size_t i = 0; i < s->ncols; i++) {
		size_t bufsize = s->defs[i].type == COL_TYPE_STRING ? s->defs[i].size : 64;
		colbuf[i] = alloca(bufsize);
		if(!colbuf[i]) {
			return 1; }
	}

	for(size_t i = 0; i < s->nrows; i++) {
		if(i) {
			append_string(",",&values);	}
		append_string("(",&values);
		for(size_t j = 0; j < s->ncols; j++) {
			size_t bufsize = s->defs[j].type == COL_TYPE_STRING ? s->defs[j].size : 64;
			if( get_column_string(colbuf[j],bufsize,&(s->defs[j]),s->valbuf[i][j]) ) {
				return 1; }
			if(j) {
				append_string(",",&values);	}
			append_string(colbuf[j],&values);
		}
		append_string(")",&values);
	}

	char fmt[] = "INSERT INTO `%s`.`%s` (%s) VALUES %s";
	size_t lenStmt = 1 + (values ? strlen(values) : 0) + strlen(fmt) + strlen(colnames) + strlen(s->defs->database) + strlen(s->defs->table);
	stmtbuf = alloca(lenStmt);
	if(!stmtbuf) {
		return 1; }
	snprintf(stmtbuf,lenStmt,fmt,s->defs->database,s->defs->table,colnames,values);
	LOGF_DEBUG("statement: %s",stmtbuf);

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stmtbuf);
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_INSERT_EXIT; }

DBI_INSERT_EXIT:
	if(colnames) {
		free(colnames); }
	if(values) {
		free(values); }
	if(dbh->dbi.result) {
		dbi_result_free(dbh->dbi.result); }
	return rc;
}

int dbi_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult* res) {
	const char fmt[] = "SELECT %s FROM `%s`.`%s` %s %s %s";
	char *colnames = 0;
	char *where = 0;
	char *stmtbuf = 0;
	char limit[32] = {0};
	int rc = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto DBI_SELECT_EXIT;	}

	if(where_string(&s->where,0,&where)) {
		rc = 1;
		goto DBI_SELECT_EXIT; }

	get_limit(s->limit, limit);

	size_t wheresize = where ? strlen(where) : 0;
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->defs[0].database) + strlen(s->defs[0].table) + strlen(colnames) + 1;
	stmtbuf = alloca(sqlsize);
	if(!stmtbuf) {
		rc = 1;
		goto DBI_SELECT_EXIT; }
	sprintf(stmtbuf,fmt,colnames,s->defs[0].database,s->defs[0].table,(where ? " WHERE " : ""),(where ? where : ""),limit);
	LOGF_DEBUG("statement:\n%s",stmtbuf);

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stmtbuf);
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_SELECT_EXIT; }

	res->cols = s->defs;
	res->ncols = s->ncols;
	res->row = malloc(sizeof(void*) * s->ncols);
	if(!res->row) {
		return 1; }
	memset(res->row,0,sizeof(void*) * s->ncols);
	for(size_t i = 0; i < s->ncols; i++) {
		size_t colsize = get_column_bufsize(&(s->defs[i]));
		if(colsize == 0) {
			LOG_WARN("invalid column size");
			rc = 1;
			goto DBI_SELECT_EXIT; }
		res->row[i] = malloc(colsize);
	}

DBI_SELECT_EXIT:
	if(colnames) {
		free(colnames); }
	if(where) {
		free(where); }
	return rc;
}

int dbi_fetch_hook(struct _DBHandle *dbh,struct _SelectResult *res) {
	if(!dbi_result_has_next_row(dbh->dbi.result)) {
		dbi_result_free(dbh->dbi.result);
		destroy_selectresult(res);
		return 0;
	}

	if(!dbi_result_next_row(dbh->dbi.result)) {
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_result_next_row(): %s",(errmsg ? errmsg :""));
		return 1; }

	for(size_t i = 0; i < res->ncols; i++) {
		if( dbi_result_field_is_null_idx(dbh->dbi.result,i) ) {

		} else {
			const DBColumnDef *col = &(res->cols[i]);
			switch(col->type) {
			case COL_TYPE_STRING:
				snprintf((char*)res->row[i],col->size,"%s",dbi_result_get_string_idx(dbh->dbi.result,i));
				break;
			case COL_TYPE_INT:
				if(col->size != 0 && col->size <= 16) {
					*((short*)res->row[i]) = dbi_result_get_short_idx(dbh->dbi.result,i);
				} else if(col->size <= 32 || col->size == 0) {
					*((long*)res->row[i]) = dbi_result_get_int_idx(dbh->dbi.result,i);
				} else if (col->size <= 64) {
					*((long long*)res->row[i]) = dbi_result_get_longlong_idx(dbh->dbi.result,i);
				} else {
					LOGF_WARN("invalid int size for mysql: %lu",col->size);
					return -1;
				}
				break;
			case COL_TYPE_FLOAT:
				*((double*)res->row[i]) = dbi_result_get_double_idx(dbh->dbi.result,i);
				break;
			case COL_TYPE_DATETIME:
			{
				time_t t = dbi_result_get_datetime_idx(dbh->dbi.result,i);
				gmtime_r(&t,((struct tm*)res->row[i]));
			}
				break;
			default:
				LOG_WARN("invalid datatype for mysql");
				return -1;
			}
		}
	}
	return 1;
}

int dbi_delete_hook(struct _DBHandle *dbh,const struct _DeleteStmt *const s) {
	char *where = 0;
	char *stmtbuf = 0;
	char limit[32] = {0};
	int rc = 0;

	get_limit(s->limit, limit);

	if(where_string(&s->where,0,&where)) {
		rc = 1;
		goto DBI_DELETE_EXIT; }

	const char fmt[] = "DELETE FROM `%s`.`%s` %s %s %s";
	size_t wheresize = where ? strlen(where) : 0;
	size_t limitsize = strlen(limit);
	size_t sqlsize = strlen(fmt) + wheresize + strlen(" WHERE ") + limitsize + strlen(s->def->database) + strlen(s->def->name) + 1;
	stmtbuf = alloca(sqlsize);
	if(!stmtbuf) {
		goto DBI_DELETE_EXIT;
		rc = 1; }
	sprintf(stmtbuf,fmt,s->def->database,s->def->name,(where ? " WHERE " : ""),(where ? where : ""),limit);
	LOGF_DEBUG("statement:\n%s",stmtbuf);

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stmtbuf);
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_DELETE_EXIT; }

DBI_DELETE_EXIT:
	if(where) {
		free(where); }
	if(dbh->dbi.result) {
		dbi_result_free(dbh->dbi.result);
		dbh->dbi.result = 0; }

	return rc;
}

int dbi_update_hook(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	return 1;
}

int dbi_upsert_hook(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	return 1;
}

#endif
