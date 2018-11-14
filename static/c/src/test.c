#ifdef _DB_TEST_VERSION

#include <stdlib.h>
#include <stdio.h>

#include "db.h"
#include "table.h"
#include "tables.h"
#include "logger.h"

static void test(DBTypes type) {
	/*
	complexdb1_complextable1 tbl;
	TABLE_STRUCT(complexdb1,complextable1,tbl)
	printf("%s\n",tbl.def->name);
	*/

	Log(LOG_DEBUG,"checking create_dbhandle()\n");
	DBHandle* dbh = create_dbhandle(type);
	if ( !dbh ) {
		Log(LOG_ERROR,"create_dbhandle() failed");
		exit(1); }

	Log(LOG_DEBUG,"checking connect_db()\n");
	if( connect_db(dbh,"localhost",3306,"mydb","myuser","mypass") ) {
		Log(LOG_ERROR,"connect_db() failed\n");
		exit(1); }

	Log(LOG_DEBUG,"checking disconnect_db()\n");
	if ( disconnect_db(dbh) ) {
		Log(LOG_ERROR,"disconnect_db() failed\n");
		exit(1); }

	Log(LOG_DEBUG,"checking destroy_dbhandle()\n");
	if( destroy_dbhandle(&dbh) ) {
		Log(LOG_ERROR,"destroy_dbhandle() failed\n");
		exit(1); }
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
