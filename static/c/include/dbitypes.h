#pragma once

#ifndef _DISABLE_DBI
typedef enum _DBIType {
	DBI_TYPE_INVALID = 0,

	DBI_TYPE_MYSQL,
	DBI_TYPE_POSTGRES,

	DBI_TYPE_MAX,
} DBIType;
#endif
