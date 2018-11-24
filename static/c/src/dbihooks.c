#ifndef _DISABLE_DBI

#include "dbihooks.h"
#include "dbhandle_impl.h"
#include "logger.h"
#include "helper.h"
#include "column.h"
#include "statements.h"
#include "selectresult.h"
#include "dbihelper.h"
#include "values.h"

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
	char *stmtbuf = 0;

	if( insert_stmt_string(s,dbi_values_specifier,&stmtbuf) ) {
		rc = 1;
		goto DBI_INSERT_EXIT; }

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stmtbuf);
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_INSERT_EXIT; }

DBI_INSERT_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
	if(dbh->dbi.result) {
		dbi_result_free(dbh->dbi.result); }
	return rc;
}

int dbi_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult* res) {
	char *stmtbuf = 0;
	int rc = 0;

	if( select_stmt_string(s,dbi_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto DBI_SELECT_EXIT; }

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
	if(stmtbuf) {
		free(stmtbuf); }
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
	char *stmtbuf = 0;
	int rc = 0;

	if( delete_stmt_string(s,dbi_where_specifier,&stmtbuf) ) {
		rc = 1;
		goto DBI_DELETE_EXIT; }

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stmtbuf);
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_DELETE_EXIT; }

DBI_DELETE_EXIT:
	if(stmtbuf) {
		free(stmtbuf); }
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
