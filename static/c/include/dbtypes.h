#pragma once

#if defined(_DISABLE_MYSQL) && defined(_DISABLE_POSTGRES) && defined(_DISABLE_DBI)
	#error enable at least one database type
#endif

typedef enum _DBTypes {
	DB_TYPE_INVALID = 0,

#ifndef _DISABLE_MYSQL
	DB_TYPE_MYSQL,
#endif

#ifndef _DISABLE_POSTGRES
	DB_TYPE_POSTGRES,
#endif

#ifndef _DISABLE_DBI
	DB_TYPE_DBI,
#endif

	DB_TYPE_MAX,
} DBTypes;

const char* dbtype_to_string(enum _DBTypes t);
