#ifdef _DB_TEST_VERSION

#include <stdlib.h>
#include <stdio.h>

#include "db.h"
#include "tables.h"
#include "logger.h"
#include "tests.h"

static DBHandle* test_create_connection(DBTypes type) {
	Log(LOG_DEBUG,"checking create_dbhandle()");
	DBHandle* dbh = create_dbhandle(type);
	if ( !dbh ) {
		LOG_FATAL(1,"create_dbhandle() failed"); }

	Log(LOG_DEBUG,"checking connect_db()");
	if( connect_db(dbh,"localhost",3306,"mydb","myuser","mypass") ) {
		LOG_FATAL(1,"connect_db() failed"); }

	return dbh;
}

static void test_destroy_connection(DBHandle** dbh) {
	Log(LOG_DEBUG,"checking disconnect_db()");
	if ( disconnect_db(*dbh) ) {
		LOG_FATAL(1,"disconnect_db() failed"); }

	Log(LOG_DEBUG,"checking destroy_dbhandle()");
	if( destroy_dbhandle(dbh) ) {
		LOG_FATAL(1,"destroy_dbhandle() failed"); }
}

static void test(DBTypes type) {
	test_tables();

	DBHandle* dbh = test_create_connection(type);
	test_destroy_connection(&dbh);
}

int main(int argc,char** argv) {

	SetLogLevel(LOG_DEBUG);

#ifndef _DISABLE_MYSQL
	test(DB_TYPE_MYSQL);
#endif
#ifndef _DISABLE_POSTGRES
	test(DB_TYPE_POSTGRES);
#endif

	return 0;
}

#endif
