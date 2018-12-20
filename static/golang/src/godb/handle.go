package godb

/*
#include <db.h>
#include <dbhandle.h>
#include <statements.h>
#include <selectresult.h>
*/
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
	
	/*
		Delete executes a DeleteStmt
	*/
	Delete(DeleteStmt)error
	
	/*
		Delete executes a SelectStmt and returns it's result
	*/
	Select(SelectStmt)(SelectResult,error)
	
	ToNative()*C.DBHandle
}

type DBHandleImpl struct {
	dbh  *C.DBHandle
	creds *C.DBCredentials
}

/*
	NewDBHandle creates a DBHandle interface with the given DBConfig conf
*/
func NewDBHandle(conf DBConfig)(DBHandle,error) {
	dbh := new(DBHandleImpl)
	if dbh.dbh = C.create_dbhandle(conf.ToNative()); dbh.dbh == nil {
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

/*
	Select implements DBHandle interface
*/
func (dbh *DBHandleImpl)Select(stmt SelectStmt)(SelectResult,error) {
	res := NewSelectResult()
	if C.select_db(dbh.dbh,stmt.ToNative(),res.ToNative()) != 0 {
		return nil,errors.New("could not select")
	}
	return res,nil
}

/*
	Delete implements DBHandle interface
*/
func (dbh *DBHandleImpl)Delete(stmt DeleteStmt)error {
	if C.delete_db(dbh.dbh,stmt.ToNative()) != 0 {
		return errors.New("could not delete")
	}
	return nil
}

func (dbh *DBHandleImpl)ToNative()*C.DBHandle {
	return dbh.dbh
}