#include "dbtypes.h"

#ifndef _DISABLE_MYSQL
static const char* c_db_type_mysql = "mysql";
#endif
#ifndef _DISABLE_POSTGRES
static const char* c_db_type_postgres = "postgres";
#endif
#ifndef _DISABLE_DBI
static const char* c_db_type_dbi = "dbi";
#endif

const char* dbtype_to_string(enum _DBTypes t) {
	switch(t) {
#ifndef _DISABLE_MYSQL
	case DB_TYPE_MYSQL:
		return c_db_type_mysql;
#endif
#ifndef _DISABLE_POSTGRES
	case DB_TYPE_POSTGRES:
		return c_db_type_postgres;
#endif
#ifndef _DISABLE_DBI
	case DB_TYPE_DBI:
		return c_db_type_dbi;
#endif
	default:
		return 0;
	}
}
