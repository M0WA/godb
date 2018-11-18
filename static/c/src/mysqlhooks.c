#ifndef _DISABLE_MYSQL

#include "mysqlhooks.h"

#include <string.h>
#include <time.h>

#include "dbhandle_impl.h"
#include "statements.h"
#include "column.h"
#include "logger.h"
#include "helper.h"

typedef struct _MySQLBindWrapper {
	MYSQL_BIND* bind;
	my_bool* is_null;
	unsigned long* str_length;
	MYSQL_TIME* times;
} MySQLBindWrapper;

static char* mysql_where(const struct _WhereClause* clause,MySQLBindWrapper* wrapper) {
	return 0;
}

static int mysql_datatype(const struct _DBColumnDef *const col,enum enum_field_types* ft,unsigned long* buffer_length) {
	*buffer_length = 0;
	switch(col->type) {
	case COL_TYPE_STRING:
		*ft = MYSQL_TYPE_STRING;
		*buffer_length = (col->size + 1);
		break;
	case COL_TYPE_INT:
		if(col->size != 0 && col->size <= 16) {
			*ft = MYSQL_TYPE_SHORT;
			*buffer_length = sizeof(short);
		} else if(col->size <= 32 || col->size == 0) {
			*ft = MYSQL_TYPE_LONG;
			*buffer_length = sizeof(long);
		} else if (col->size <= 64) {
			*ft = MYSQL_TYPE_LONGLONG;
			*buffer_length = sizeof(long long);
		} else {
			LOGF_WARN("invalid int size for mysql: %lu",col->size);
			return 1;
		}
		break;
	case COL_TYPE_FLOAT:
		*ft = MYSQL_TYPE_DOUBLE;
		*buffer_length = sizeof(double);
		break;
	case COL_TYPE_DATETIME:
		*ft = MYSQL_TYPE_TIMESTAMP;
		*buffer_length = sizeof(MYSQL_TIME);
		break;
	default:
		LOG_WARN("invalid datatype for mysql");
		return 1;
	}
	return 0;
}

static int mysql_time(const struct tm *const t,MYSQL_TIME* mt) {
	mt->year=t->tm_year + 1900;
	mt->month=t->tm_mon;
	mt->day=t->tm_mday;
	mt->hour=t->tm_hour;
	mt->minute=t->tm_min;
	mt->second=t->tm_sec;
	return 0;
}

int mysql_connect_hook(struct _DBHandle* dbh) {
	if(!dbh) {
		LOG_WARN("handle is null");
		return 1;
	}
	if(dbh->mysql.conn) {
		LOG_WARN("mysql handle is already initalized");
		return 1;
	}
	if(! (dbh->mysql.conn = mysql_init(dbh->mysql.conn)) ) {
		LOG_WARN("mysql handle failed to initalize");
		return 1;
	}

	my_bool one = 1;
	if(dbh->config.mysql.autoreconnect) {
		mysql_options(dbh->mysql.conn,MYSQL_OPT_RECONNECT,(const char*)&one); }
	if(dbh->config.mysql.compression) {
		mysql_options(dbh->mysql.conn,MYSQL_OPT_COMPRESS, (const char*)&one); }

	dbh->mysql.conn = mysql_real_connect(
			dbh->mysql.conn,
			dbh->config.host,
			dbh->config.user,
			dbh->config.pass,
			dbh->config.name,
			dbh->config.port,
			NULL,
			CLIENT_MULTI_STATEMENTS |
			CLIENT_COMPRESS |
			CLIENT_IGNORE_SIGPIPE );

	if(!dbh->mysql.conn) {
		LOGF_WARN("mysql connect: %s",mysql_error(dbh->mysql.conn));
		return 1;
	}
	mysql_autocommit(dbh->mysql.conn,1);

	LOG_DEBUG("connected to mysql-database");
	return 0;
}

int mysql_disconnect_hook(struct _DBHandle* dbh) {
	if(!dbh) {
		LOG_WARN("handle is null");
		return 1;
	}
	if(!dbh->mysql.conn) {
		LOG_WARN("mysql handle is not initalized");
		return 1;
	}
	mysql_close(dbh->mysql.conn);
	dbh->mysql.conn = 0;

	LOG_DEBUG("disconnected from mysql-database");
	return 0;
}

int mysql_insert_hook(struct _DBHandle* dbh,const struct _InsertStmt *const s) {
	int rc = 0;
	char* colnames = 0;
	char stmtbuf[4096] = {0};
	MYSQL_STMT *mystmt = 0;
	char* values = 0;
	size_t time_cols = 0, str_cols = 0;

	struct _MySQLBindWrapper bind;
	bind.bind = 0;
	bind.is_null = 0;
	bind.str_length = 0;
	bind.times = 0;

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}

	size_t valsize = (s->ncols * 2) + 3 + 1;
	values = malloc(valsize);
	if(!values) {
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}
	values[0] = 0;
	strcat(values,"(");

	for(size_t i = 0; i < s->nrows; i++) {
		for(size_t j = 0; j < s->ncols; j++) {
			if(s->defs[j].type == COL_TYPE_DATETIME) {
				time_cols++;
			} else if (s->defs[j].type == COL_TYPE_STRING) {
				str_cols++;
			}

			if(j) {
				strcat(values,",");
			}
			strcat(values,"?");
		}
		strcat(values,")");

		if(s->nrows > 1 && i < (s->nrows-1)) {
			strcat(values,",(");
		}
	}

	char fmt[] = "INSERT INTO `%s`.`%s` (%s) VALUES %s";
	sprintf(stmtbuf,fmt,s->defs->database,s->defs->table,colnames,values);

	mystmt = mysql_stmt_init(dbh->mysql.conn);
	if(!mystmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_INSERT_EXIT; }

	int err = mysql_stmt_prepare(mystmt, stmtbuf, strlen(stmtbuf));
	if(err) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}

	if( mysql_stmt_param_count(mystmt) != (s->ncols * s->nrows) ) {
		LOG_WARN("mysql_stmt_param_count: invalid column count in prepared statement");
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}

	bind.bind = malloc(sizeof(MYSQL_BIND) * s->ncols * s->nrows);
	if(!bind.bind) {
		rc = 1;
		goto MYSQL_INSERT_EXIT; }
	memset(bind.bind,0,sizeof(MYSQL_BIND) * s->ncols * s->nrows);

	bind.is_null = malloc(sizeof(my_bool) * s->ncols * s->nrows);
	if(!bind.is_null) {
		rc = 1;
		goto MYSQL_INSERT_EXIT; }
	memset(bind.is_null,0,sizeof(my_bool) * s->ncols * s->nrows);

	if(str_cols) {
		bind.str_length = malloc(sizeof(unsigned long) * str_cols);
		if(!bind.str_length) {
			rc = 1;
			goto MYSQL_INSERT_EXIT; }
		memset(bind.str_length,0,sizeof(unsigned long) * str_cols);
	}
	if(time_cols) {
		bind.times = malloc(sizeof(MYSQL_TIME) * time_cols);
		if(!bind.times) {
			rc = 1;
			goto MYSQL_INSERT_EXIT; }
		memset(bind.times,0,sizeof(MYSQL_TIME) * time_cols);
	}

	for(size_t i = 0; i < s->nrows; i++) {
		for(size_t j = 0,timeidx = 0,stridx = 0; j < s->ncols; j++) {
			size_t idxbind = (i * s->ncols) + j;
			if( mysql_datatype(&s->defs[j],&bind.bind[idxbind].buffer_type,&bind.bind[idxbind].buffer_length) ) {
				rc = 1;
				goto MYSQL_INSERT_EXIT;
			}
			if(!s->valbuf[i][j]) {
				bind.is_null[idxbind] = 1;

				bind.bind[idxbind].is_null = &bind.is_null[idxbind];
				bind.bind[idxbind].buffer = 0;
				bind.bind[idxbind].length = 0;
			} else if(s->defs[j].type == COL_TYPE_DATETIME) {
				if( mysql_time((const struct tm *const)(s->valbuf[i][j]),&bind.times[timeidx]) ) {
					rc = 1;
					goto MYSQL_INSERT_EXIT;
				}
				bind.bind[idxbind].buffer = &bind.times[timeidx];
				bind.bind[idxbind].length = 0;
				timeidx++;
			} else {
				bind.bind[idxbind].is_null = 0;
				bind.bind[idxbind].buffer = (void *)(s->valbuf[i][j]);
				bind.bind[idxbind].length = 0;

				if(s->defs[j].type == COL_TYPE_STRING) {
					bind.str_length[stridx] = strlen((char *)(s->valbuf[i][j])) + 1;
					bind.bind[idxbind].length = &bind.str_length[stridx];
					stridx++;
				}
			}
		}
	}
	if( mysql_stmt_bind_param(mystmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;	}
	if( mysql_stmt_execute(mystmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_INSERT_EXIT;
	}

MYSQL_INSERT_EXIT:
	if(colnames) {
		free(colnames);	}
	if(bind.bind) {
		free(bind.bind); }
	if(bind.is_null) {
		free(bind.is_null); }
	if(bind.str_length) {
		free(bind.str_length); }
	if(bind.times) {
		free(bind.times); }
	if(values) {
		free(values); }
	if(mystmt) {
		mysql_stmt_close(mystmt); }
	return rc;
}

int mysql_update_hook(struct _DBHandle* dbh,const struct _UpdateStmt *const s) {
	return 1;
}

int mysql_upsert_hook(struct _DBHandle* dbh,const struct _UpsertStmt *const s) {
	return 1;
}

int mysql_delete_hook(struct _DBHandle* dbh,const struct _DeleteStmt *const s) {
	return 1;
}

struct _SelectResult* mysql_select_hook(struct _DBHandle* dbh,const struct _SelectStmt *const s) {
	static const char fmt[] = "SELECT %s FROM `%s`.`%s` %s";
	char* colnames = 0;
	char* where = 0;
	char* stmtbuf = 0;
	int rc = 0;
	MYSQL_STMT *mystmt = 0;
	MySQLBindWrapper bind;
	memset(&bind,0,sizeof(MySQLBindWrapper));

	colnames = comma_concat_colnames(s->defs,s->ncols);
	if(!colnames) {
		rc = 1;
		goto MYSQL_SELECT_EXIT;
	}

	where = mysql_where(&s->where,&bind);
	if(!where) {
		rc = 1;
		goto MYSQL_SELECT_EXIT;
	}

	size_t sqlsize = strlen(fmt) + strlen(where) + strlen(colnames);
	stmtbuf = malloc(sqlsize);
	if(!stmtbuf) {
		rc = 1;
		goto MYSQL_SELECT_EXIT;
	}
	sprintf(stmtbuf,fmt,colnames,s->defs[0].database,s->defs[0].table,where);

	mystmt = mysql_stmt_init(dbh->mysql.conn);
	if(!mystmt) {
		rc = 1;
		LOG_WARN("mysql_stmt_init: is null");
		goto MYSQL_SELECT_EXIT; }

	if( mysql_stmt_prepare(mystmt, stmtbuf, strlen(stmtbuf)) ) {
		LOGF_DEBUG("%s",stmtbuf);
		LOGF_WARN("mysql_stmt_prepare: %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	if( bind.bind && mysql_stmt_bind_param(mystmt,bind.bind) ) {
		LOGF_WARN("mysql_bind_param(): %s",mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	if( mysql_stmt_execute(mystmt) ) {
		LOGF_WARN("mysql_stmt_execute(): %s", mysql_stmt_error(mystmt));
		rc = 1;
		goto MYSQL_SELECT_EXIT;	}

	//TODO: read result

MYSQL_SELECT_EXIT:
	if(colnames) {
		free(colnames); }
	if(stmtbuf) {
		free(stmtbuf); }
	if(mystmt) {
		mysql_stmt_close(mystmt); }
	if(where) {
		free(where); }
	if(bind.bind) {
		free(bind.bind); }
	if(bind.is_null) {
		free(bind.is_null); }
	if(bind.str_length) {
		free(bind.str_length); }
	if(bind.times) {
		free(bind.times); }

	if(rc == 1) {
		return 0;}
	return 0;
}

#endif
