package godb

// #include <db.h>
// #include <dbhandle.h>
import "C"

import (
	"errors"
)

type DBHandle interface {
	/*
		Connect connects to a database using given credentials
	*/
	Connect(DBCredentials)error
	
	/*
		Disconnect disconnects from a connected database
	*/
	Disconnect()error
	
	ToNative()*C.DBHandle
}

type DBHandleImpl struct {
	conf *C.DBConfig
	dbh  *C.DBHandle
	creds *C.DBCredentials
}

/*
	NewDBHandle creates a DBHandle interface with the given DBConfig conf
*/
func NewDBHandle(conf DBConfig)(DBHandle,error) {
	dbh := new(DBHandleImpl)
	dbh.conf = conf.ToNative()
	if dbh.dbh = C.create_dbhandle(dbh.conf); dbh.dbh == nil {
		return nil,errors.New("could not create db handle")
	}
	return dbh,nil
}

/*
	Connect implements DBHandle interface
*/
func (dbh *DBHandleImpl)Connect(creds DBCredentials)error {
	dbh.creds = creds.ToNative()
	if C.connect_db(dbh.dbh,dbh.creds) != 0 {
		return errors.New("could not connect to database")
	}
	return nil
}

/*
	Disconnect implements DBHandle interface
*/
func (dbh *DBHandleImpl)Disconnect()error {
	if C.disconnect_db(dbh.dbh) != 0 {
		return errors.New("could not connect to database")
	}
	dbh.creds = nil
	return nil
}

func (dbh *DBHandleImpl)ToNative()*C.DBHandle {
	return dbh.dbh
}