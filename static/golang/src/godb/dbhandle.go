package godb

// #include <db.h>
// #include <dbhandle.h>
import "C"

import (
	"errors"
)

type DBHandler interface {
	Connect(DBCredentials)(error)
	Disconnect()(error)
}

type DBHandle struct {
	conf *C.DBConfig
	dbh  *C.DBHandle
	creds *C.DBCredentials
}

func NewDBHandle(conf DBConfig)(DBHandler,error) {
	dbh := new(DBHandle)
	dbh.conf = conf.ToNative()
	if dbh.dbh = C.create_dbhandle(dbh.conf); dbh.dbh == nil {
		return nil,errors.New("could not create db handle")
	}
	return dbh,nil
}

func (dbh *DBHandle)Connect(creds DBCredentials)(error) {
	dbh.creds = creds.ToNative()
	if C.connect_db(dbh.dbh,dbh.creds) != 0 {
		return errors.New("could not connect to database")
	}
	return nil
}

func (dbh *DBHandle)Disconnect()(error) {
	if C.disconnect_db(dbh.dbh) != 0 {
		return errors.New("could not connect to database")
	}
	dbh.creds = nil
	return nil
}