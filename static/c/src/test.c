#ifdef DB_TEST_VERSION

#include <stdlib.h>

#include "db.h"

static void test(DBTypes type) {
	DBHandle* dbh = create_dbhandle(type);
	if ( !dbh ) {
		exit(1); }

	if( connect_db(dbh,"localhost",3306,"mydb","myuser","mypass") ) {
		exit(1); }

	if ( disconnect_db(dbh) ) {
		exit(1); }

	if( destroy_dbhandle(&dbh) ) {
		exit(1); }
}

int main(int argc,char** argv) {

#ifndef _DISABLE_MYSQL
	test(DB_TYPE_MYSQL);
#endif
#ifndef _DISABLE_POSTGRES
	test(DB_TYPE_POSTGRES);
#endif

	return 0;
}

#endif
