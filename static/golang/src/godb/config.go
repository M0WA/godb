package godb

/*
#include <stdlib.h>

#include <db.h>
#include <dbtypes.h>
#include <dbitypes.h>
#include <dbhandle.h>

static void set_dbi_type_mysql(DBConfig *conf) {
	conf->Dbi.Type = DBI_TYPE_MYSQL;
}

static void set_dbi_type_postgres(DBConfig *conf) {
	conf->Dbi.Type = DBI_TYPE_POSTGRES;
}

static DBConfig* alloc_dbconfig() {
	return malloc(sizeof(struct _DBConfig));
}

static void free_dbconfig(DBConfig *conf) {
	if(conf) {
		free(conf);	}
}

*/
import "C"

import (
	"errors"
	"runtime"
)

type DBType int

const (
	INVALID      DBType = iota
	MYSQL        DBType = iota
	POSTGRES     DBType = iota
	DBI_MYSQL    DBType = iota
	DBI_POSTGRES DBType = iota
)

type DBConfig interface {
	CDBConfig()(*C.DBConfig)
}

type DBConfigImpl struct {
	native *C.DBConfig
}

func freeDBConfig(c *DBConfigImpl) {
	C.free_dbconfig(c.native)
}

/*
	NewDBConfig creates a DBConfig interface for a given type dbtype
*/
func NewDBConfig(dbtype DBType)(DBConfig,error) {
	c := new(DBConfigImpl)
	c.native = C.alloc_dbconfig()
	switch dbtype {
		case MYSQL:
		c.native.Type = C.DB_TYPE_MYSQL
		case POSTGRES:
		c.native.Type = C.DB_TYPE_POSTGRES
		case DBI_MYSQL:
		c.native.Type = C.DB_TYPE_DBI
		C.set_dbi_type_mysql(c.native)
		case DBI_POSTGRES:
		c.native.Type = C.DB_TYPE_DBI
		C.set_dbi_type_postgres(c.native)
		default:
		return nil, errors.New("invalid database type")
	}
	runtime.SetFinalizer(c, freeDBConfig)
	return c,nil
}

/*
	CDBConfig implements DBConfig interface
*/
func (c *DBConfigImpl)CDBConfig()*C.DBConfig {
	return c.native
}