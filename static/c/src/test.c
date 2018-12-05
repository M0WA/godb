#ifdef _DB_TEST_VERSION

#include <stdlib.h>
#include <stdio.h>
#include <mcheck.h>

#include "db.h"
#include "tables.h"
#include "column.h"
#include "where.h"
#include "logger.h"
#include "statements.h"
#include "tests.h"
#include "dbhandle.h"

#ifndef _DISABLE_DBI
#define ENABLE_MYSQL_CRED
#define ENABLE_POSTGRES_CRED
#endif

#ifndef _DISABLE_MYSQL
#define ENABLE_MYSQL_CRED
#endif

#ifndef _DISABLE_POSTGRES
#define ENABLE_POSTGRES_CRED
#endif

#ifdef ENABLE_MYSQL_CRED
static const DBCredentials mysql_creds = (DBCredentials) {
	.host = "localhost",
	.port = 3306,
	.name = "complexdb1",
	.user = "myuser",
	.pass = "mypass",
};
#endif

#ifdef ENABLE_POSTGRES_CRED
static const DBCredentials postgres_creds = (DBCredentials) {
	.host = "localhost",
	.port = 5432,
	.name = "complexdb1",
	.user = "myuser",
	.pass = "mypass",
};
#endif

#ifndef _DISABLE_MYSQL
static DBConfig mysql_conf = (DBConfig) {
	.type = DB_TYPE_MYSQL,
	.mysql.compression = 1,
	.mysql.autoreconnect = 1,
	.mysql.preparedstatements = 1,
};
#endif
#ifndef _DISABLE_POSTGRES
static DBConfig postgres_conf = (DBConfig) {
	.type = DB_TYPE_POSTGRES,
	.postgres.preparedstatements = 0,
};
#endif
#ifndef _DISABLE_DBI
#include "dbitypes.h"
static DBConfig dbi_conf = (DBConfig) {
	.type = DB_TYPE_DBI,
    .dbi.type = DBI_TYPE_INVALID,
};
#endif

static DBHandle* test_create_connection(const DBConfig *conf,const DBCredentials *creds) {
	LOG_DEBUG("checking create_dbhandle()");

	DBHandle *dbh = create_dbhandle(conf);
	if ( !dbh ) {
		LOG_FATAL(1,"create_dbhandle() failed"); }

	LOG_DEBUG("checking connect_db()");
	if( connect_db(dbh,creds) ) {
		LOG_FATAL(1,"connect_db() failed"); }

	return dbh;
}

static void test_destroy_connection(DBHandle* dbh) {
	LOG_DEBUG("checking disconnect_db()");
	if ( disconnect_db(dbh) ) {
		LOG_FATAL(1,"disconnect_db() failed"); }

	LOG_DEBUG("checking destroy_dbhandle()");
	if( destroy_dbhandle(dbh) ) {
		LOG_FATAL(1,"destroy_dbhandle() failed"); }
}

static void test_where() {
	DBColumnDef cols[] = {
		(DBColumnDef){
			.type = COL_TYPE_STRING,
			.name = "testcol",
			.table = "testtable",
			.database = "testdb",
			.autoincrement = 0,
			.notnull = 1,
			.size = 255,
		},
		(DBColumnDef){
			.type = COL_TYPE_INT,
			.name = "testcol2",
			.table = "testtable",
			.database = "testdb",
			.autoincrement = 0,
			.notnull = 1,
			.size = 8,
		}
	};

	struct _values_str {
		char c1[255];
		char c2[255];
	};
	struct _values_str strs;
	sprintf(strs.c1,"test1");
	sprintf(strs.c2,"test2");

	struct _values_short {
		short s1;
		short s2;
	};
	struct _values_short shorts;
	shorts.s1 = 10; shorts.s2 = 5;

	WhereCondition cond[] = {
		(WhereCondition) {
			.type = WHERE_COND,
			.cond = WHERE_EQUAL,
			.def = &cols[0],
			.values = (const void**)(&strs),
			.cnt = 2,
		},
		(WhereCondition) {
			.type = WHERE_COND,
			.cond = WHERE_EQUAL,
			.def = &cols[1],
			.values = (const void**)(&shorts),
			.cnt = 2,
		},
	};

	WhereClause conds;
	memset(&conds,0,sizeof(WhereClause));

	if( where_append(&conds,(WhereStmt*)&cond[0]) ) {
		LOG_FATAL(1,"where_append() failed"); }
	if( where_append(&conds,(WhereStmt*)&cond[1]) ) {
		LOG_FATAL(1,"where_append() failed"); }

	WhereComposite comp = {
		.type = WHERE_COMP,
		.where = 0,
		.cnt = 0,
	};
	if( where_comp_append(&comp,&conds) ) {
		LOG_FATAL(1,"where_comp_append() failed");}

	WhereClause clause;
	memset(&clause,0,sizeof(WhereClause));
	if( where_append(&clause,(WhereStmt*)&comp) ) {
		LOG_FATAL(1,"where_append() failed"); }
	if( where_append(&clause,(WhereStmt*)&comp) ) {
		LOG_FATAL(1,"where_append() failed"); }

	where_destroy(&clause);
}

static void test_delete() {
	struct _values_str {
		char c1[255];
	};
	struct _values_str strs;
	sprintf(strs.c1,"test1");

	DBColumnDef col = (DBColumnDef){
		.type = COL_TYPE_STRING,
		.name = "testcol",
		.table = "testtable",
		.database = "testdb",
		.autoincrement = 0,
		.notnull = 1,
		.size = 255,
	};

	DBTableDef tbl = (DBTableDef){
		.name = "testtable",
		.database = "testdb",
		.cols = &col,
		.ncols = 1,
	};

	WhereCondition cond = (WhereCondition) {
		.type = WHERE_COND,
		.cond = WHERE_EQUAL,
		.def = &col,
		.values = (const void**)(&strs),
		.cnt = 1,
	};

	DeleteStmt stmt;
	memset(&stmt,0,sizeof(DeleteStmt));
	stmt.def = &tbl;
	if( where_append(&stmt.where,(WhereStmt*)&cond) ) {
		LOG_FATAL(1,"where_append() failed"); }

	DESTROY_STMT(&stmt);
}

static void test(const DBConfig *conf,const DBCredentials *creds) {
	DBHandle *dbh = test_create_connection(conf,creds);
	test_tables_db(dbh);
	test_destroy_connection(dbh);
}

int main(int argc,char** argv) {
	mtrace ();

	set_loglevel(LOGLVL_DEBUG);
	init_dblib();

	LOG_DEBUG("checking generated tables");
	test_tables_static();

	LOG_DEBUG("checking where statements");
	test_where();

	LOG_DEBUG("checking delete statements");
	test_delete();

	for(DBTypes i = DB_TYPE_INVALID + 1; i < DB_TYPE_MAX; i++) {
		const char *dbtypestr = dbtype_to_string(i);
		LOGF_DEBUG("======================== checking db type %s =========================",dbtypestr);

		switch(i) {
#ifndef _DISABLE_MYSQL
		case DB_TYPE_MYSQL:
			LOG_DEBUG("checking mysql raw statements");
			mysql_conf.mysql.preparedstatements = 0;
			test(&mysql_conf,&mysql_creds);
			LOG_DEBUG("checking mysql prepared statements");
			mysql_conf.mysql.preparedstatements = 1;
			test(&mysql_conf,&mysql_creds);
			break;
#endif
#ifndef _DISABLE_POSTGRES
		case DB_TYPE_POSTGRES:
			LOG_DEBUG("checking postgres raw statements");
			postgres_conf.postgres.preparedstatements = 0;
			test(&postgres_conf,&postgres_creds);
			LOG_DEBUG("checking postgres prepared statements");
			postgres_conf.postgres.preparedstatements = 1;
			test(&postgres_conf,&postgres_creds);
			break;
#endif
#ifndef _DISABLE_DBI
		case DB_TYPE_DBI:
			for(DBIType dbi = (DBI_TYPE_INVALID + 1); dbi < DBI_TYPE_MAX; dbi++) {
				switch(dbi) {
				case DBI_TYPE_MYSQL:
					LOG_DEBUG("checking dbi mysql");
				{
					DBConfig ctmp = dbi_conf;
					ctmp.dbi.type = DBI_TYPE_MYSQL;
					test(&ctmp,&mysql_creds);
				}
				case DBI_TYPE_POSTGRES:
					LOG_DEBUG("checking dbi postgres");
				{
					DBConfig ctmp = dbi_conf;
					ctmp.dbi.type = DBI_TYPE_POSTGRES;
					test(&ctmp,&postgres_creds);
				}
					break;
				default:
					LOG_FATAL(1,"invalid dbitype");
					break;
				}
			}
			break;
#endif
		default:
			LOG_FATAL(1,"invalid dbtype");
			break;
		}
		LOGF_DEBUG("======================== checking db type %s: SUCCESS =========================",dbtypestr);
	}

	exit_dblib();
	muntrace ();
	return 0;
}
#endif
