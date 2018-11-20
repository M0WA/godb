#ifdef _DB_TEST_VERSION

#include <stdlib.h>
#include <stdio.h>

#include "db.h"
#include "tables.h"
#include "logger.h"
#include "tests.h"

static DBHandle* test_create_connection(DBTypes type) {
	LOG_DEBUG("checking create_dbhandle()");
	DBHandle* dbh = create_dbhandle(type);
	if ( !dbh ) {
		LOG_FATAL(1,"create_dbhandle() failed"); }

	LOG_DEBUG("checking connect_db()");
	if( connect_db(dbh,"localhost",3306,"mydb","myuser","mypass") ) {
		LOG_FATAL(1,"connect_db() failed"); }

	return dbh;
}

static void test_destroy_connection(DBHandle** dbh) {
	LOG_DEBUG("checking disconnect_db()");
	if ( disconnect_db(*dbh) ) {
		LOG_FATAL(1,"disconnect_db() failed"); }

	LOG_DEBUG("checking destroy_dbhandle()");
	if( destroy_dbhandle(dbh) ) {
		LOG_FATAL(1,"destroy_dbhandle() failed"); }
}

static void test(DBTypes type) {
	DBHandle* dbh = test_create_connection(type);
	test_tables_db(dbh);
	test_destroy_connection(&dbh);
}

int main(int argc,char** argv) {

	set_loglevel(LOGLVL_DEBUG);
	init_dblib();

	LOG_DEBUG("checking generated tables");
	test_tables_static();

#ifndef _DISABLE_MYSQL
	LOG_DEBUG("checking db type mysql");
	test(DB_TYPE_MYSQL);
#endif
#ifndef _DISABLE_POSTGRES
	LOG_DEBUG("checking db type postgres");
	test(DB_TYPE_POSTGRES);
#endif

	exit_dblib();
	return 0;
}
#endif
