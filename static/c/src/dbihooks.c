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
#include "stringbuf.h"

#include <string.h>

static dbi_inst dbiinst;

int dbi_initlib_hook() {
	dbi_initialize_r(NULL, &dbiinst);
	return 0;
}

int dbi_exitlib_hook() {
    dbi_shutdown_r(dbiinst);
	return 0;
}

int dbi_create_hook(struct _DBHandle *dbh) {
	dbh->dbi.result = 0;
	switch(dbh->config.dbi.type) {
	case DBI_TYPE_MYSQL:
		dbh->dbi.conn = dbi_conn_new_r("mysql", dbiinst);
		break;
	case DBI_TYPE_POSTGRES:
		dbh->dbi.conn = dbi_conn_new_r("pgsql", dbiinst);
		break;
	default:
		LOG_WARN("invalid dbi type");
		return 1;
	}
	if(!dbh->dbi.conn) {
		LOG_WARN("could not create dbi connection");
		return 1;
	}
	return 0;
}

int dbi_destroy_hook(struct _DBHandle* dbh) {
	return 0;
}

int dbi_connect_hook(struct _DBHandle *dbh) {
    dbi_conn_set_option(dbh->dbi.conn, "host", dbh->cred.host /* dbh->config.port */);
    dbi_conn_set_option(dbh->dbi.conn, "username", dbh->cred.user);
    dbi_conn_set_option(dbh->dbi.conn, "password", dbh->cred.pass);
    dbi_conn_set_option(dbh->dbi.conn, "dbname", dbh->cred.name);
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
	dbh->dbi.conn = 0;
	return 0;
}

int dbi_insert_hook(struct _DBHandle *dbh,const struct _InsertStmt *const s) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	int skip_autoincrement = 0;
	if(dbh->config.dbi.type == DBI_TYPE_POSTGRES) {
		skip_autoincrement = 1;	}

	if( insert_stmt_string(s,values_generic_value_specifier,&stmtbuf,skip_autoincrement) ) {
		rc = 1;
		goto DBI_INSERT_EXIT; }

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stringbuf_get(&stmtbuf));
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_INSERT_EXIT; }

DBI_INSERT_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(dbh->dbi.result) {
		dbi_result_free(dbh->dbi.result); }
	return rc;
}

int dbi_select_hook(struct _DBHandle *dbh,const struct _SelectStmt *const s,struct _SelectResult* res) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( select_stmt_string(s,where_generic_value_specifier,&stmtbuf) ) {
		rc = 1;
		goto DBI_SELECT_EXIT; }

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stringbuf_get(&stmtbuf));
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_SELECT_EXIT; }

	if( create_selectresult(s->defs,s->ncols,res) ) {
		LOG_WARN("create_selectresult(): could not create select stmt");
		rc = 1;
		goto DBI_SELECT_EXIT;}

DBI_SELECT_EXIT:
	stringbuf_destroy(&stmtbuf);
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
		return -1;
	}

	if(dbi_result_get_numfields(dbh->dbi.result) != res->ncols) {
		LOG_WARN("invalid field count in result")
		return -1;
	}

	for(size_t residx = 0; residx < res->ncols; residx++) {
		int found = 0;
		size_t dbiidx = 1;
		for(; dbiidx <= res->ncols; dbiidx++) {
			if(strcasecmp(dbi_result_get_field_name(dbh->dbi.result,dbiidx),res->cols[residx].name) == 0) {
				found = 1;
				break;
			}
		}
		if(!found) {
			LOGF_WARN("missing column %s in result",res->cols[residx].name);
			return -1;
		}

		if( dbi_result_field_is_null_idx(dbh->dbi.result,dbiidx) ) {
			res->row[residx] = 0;
		} else {
			const DBColumnDef *col = &(res->cols[residx]);

			switch(col->type) {
			case COL_TYPE_STRING:
				snprintf((char*)res->row[residx],col->size,"%s",dbi_result_get_string_idx(dbh->dbi.result,dbiidx));
				break;
			case COL_TYPE_INT:
				if(col->size != 0 && col->size <= sizeof(short)) {
					short tmpint = col->notsigned ? dbi_result_get_short_idx(dbh->dbi.result,dbiidx) : dbi_result_get_ushort_idx(dbh->dbi.result,dbiidx);
					*((short*)res->row[residx]) = tmpint;
				} else if((col->size <= sizeof(long) && sizeof(long long) != sizeof(long)) || col->size == 0) {
					long tmpint = col->notsigned ? dbi_result_get_int_idx(dbh->dbi.result,dbiidx) : dbi_result_get_uint_idx(dbh->dbi.result,dbiidx);
					*((long*)res->row[residx]) = tmpint;
				} else if (col->size <= sizeof(long long)) {
					long long tmpint = col->notsigned ? dbi_result_get_longlong_idx(dbh->dbi.result,dbiidx) : dbi_result_get_ulonglong_idx(dbh->dbi.result,dbiidx);
					*((long long*)res->row[residx]) = tmpint;
				} else {
					LOGF_WARN("invalid int size for dbi: %lu",col->size);
					return -1;
				}
				break;
			case COL_TYPE_FLOAT:
			{
				double tmpfloat = dbi_result_get_double_idx(dbh->dbi.result,dbiidx);
				*((double*)res->row[residx]) = tmpfloat;
			}
				break;
			case COL_TYPE_DATETIME:
			{
				time_t t = dbi_result_get_datetime_idx(dbh->dbi.result,dbiidx);
				gmtime_r(&t,((struct tm*)res->row[residx]));
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
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( delete_stmt_string(s,where_generic_value_specifier,&stmtbuf) ) {
		rc = 1;
		goto DBI_DELETE_EXIT; }

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stringbuf_get(&stmtbuf));
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_DELETE_EXIT; }

DBI_DELETE_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(dbh->dbi.result) {
		dbi_result_free(dbh->dbi.result);
		dbh->dbi.result = 0; }

	return rc;
}

int dbi_update_hook(struct _DBHandle *dbh,const struct _UpdateStmt *const s) {
	int rc = 0;

	StringBuf stmtbuf;
	stringbuf_init(&stmtbuf,SQL_STMT_ALLOC_BLOCK);

	if( update_stmt_string(s,values_generic_value_specifier,where_generic_value_specifier,&stmtbuf,1) ) {
		rc = 1;
		goto DBI_UPDATE_EXIT; }

	dbh->dbi.result = dbi_conn_query(dbh->dbi.conn,stringbuf_get(&stmtbuf));
	if(!dbh->dbi.result) {
		rc = 1;
		const char* errmsg = 0;
		dbi_conn_error(dbh->dbi.conn, &errmsg);
		LOGF_WARN("error while dbi_conn_query(): %s",(errmsg ? errmsg :""));
		goto DBI_UPDATE_EXIT; }

DBI_UPDATE_EXIT:
	stringbuf_destroy(&stmtbuf);
	if(dbh->dbi.result) {
		dbi_result_free(dbh->dbi.result);
		dbh->dbi.result = 0; }
	return rc;
}

int dbi_upsert_hook(struct _DBHandle *dbh,const struct _UpsertStmt *const s) {
	//int dbi_upsert_stmt_string(const struct _DBHandle *dbh,const UpsertStmt *const s, char** sql);
	return 1;
}

#endif
