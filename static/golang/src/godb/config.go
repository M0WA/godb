package godb

// #include <db.h>
// #include <dbtypes.h>
import "C"

import (
	"errors"
)

type DBType int

const (
	INVALID      DBType = iota
	MYSQL        DBType = iota
	POSTGRES     DBType = iota
	DBI          DBType = iota
)

type DBConfig interface {
	ToNative()(*C.DBConfig)
}

type DBConfigImpl struct {
	native C.DBConfig
}

/*
	NewDBConfig creates a DBConfig interface for a given type dbtype
*/
func NewDBConfig(dbtype DBType)(DBConfig,error) {
	c := new(DBConfigImpl)
	switch dbtype {
		case MYSQL:
		c.native.Type = C.DB_TYPE_MYSQL
		case POSTGRES:
		c.native.Type = C.DB_TYPE_POSTGRES
		case DBI:
		c.native.Type = C.DB_TYPE_DBI
		default:
		return nil, errors.New("invalid database type")
	}
	return c,nil
}

/*
	ToNative implements DBConfig interface
*/
func (c *DBConfigImpl)ToNative()(*C.DBConfig) {
	return &c.native
}