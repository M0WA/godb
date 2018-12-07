package godb

// #include <db.h>
import "C"

import (
	"errors"
)

type DBHandler interface {
	Connect()(error)
	Disconnect()(error)
}

type DBHandle struct {
	conf DBConfig
	dbh  *C.DBHandle
}

func New(conf DBConfig)(DBHandler,error) {
	dbh := new(DBHandle)
	dbh.conf = conf
	if dbh.dbh = C.create_dbhandle(dbh.conf.ToNative()); dbh.dbh == nil {
		return nil,errors.New("could not create db handle")
	}
	return dbh,nil
}

func (dbh *DBHandle)Connect()(error) {
	if C.connect_db(dbh.dbh,dbh.conf.ToCredentials()) != 0 {
		return errors.New("could not connect to database")
	}
	return nil
}

func (dbh *DBHandle)Disconnect()(error) {
	if C.disconnect_db(dbh.dbh) != 0 {
		return errors.New("could not connect to database")
	}
	return nil
}