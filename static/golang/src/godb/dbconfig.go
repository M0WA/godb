package godb

// #include <db.h>
import "C"

import (
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
	ToNative()(*C.DBConfig)
}

type DBConfigImpl struct {
	dbtype DBType
}

func NewDBConf(dbtype DBType)(DBConfig,error) {
	c := new(DBConfigImpl)
	c.dbtype = dbtype
	return c,nil
}