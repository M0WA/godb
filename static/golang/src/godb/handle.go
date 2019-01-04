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
		Insert inserts one row into a table
	*/
	Insert(InsertStmt)error
	
	/*
		Update updates rows in a table
	*/
	Update(UpdateStmt)error
	
	/*
		Upsert updates/inserts rows in a table
	*/
	Upsert(UpsertStmt)error
	
	/*
		Delete deletes rows from a table
	*/
	Delete(DeleteStmt)error
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
	if dbh.dbh = C.create_dbhandle(conf.CDBConfig()); dbh.dbh == nil {
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
	Insert implements DBHandle interface
*/
func (dbh *DBHandleImpl)Insert(s InsertStmt)error {
	if C.insert_db(dbh.dbh,s.CInsertStmt()) != 0 {
		return errors.New("could not insert into table")
	}
	return nil
}

/*
	Update implements DBHandle interface
*/
func (dbh *DBHandleImpl)Update(s UpdateStmt)error {
	if C.update_db(dbh.dbh,s.CUpdateStmt()) != 0 {
		return errors.New("could not update table")
	}
	return nil
}

/*
	Upsert implements DBHandle interface
*/
func (dbh *DBHandleImpl)Upsert(s UpsertStmt)error {
	if C.upsert_db(dbh.dbh,s.CUpsertStmt()) != 0 {
		return errors.New("could not upsert into table")
	}
	return nil
}

/*
	Delete implements DBHandle interface
*/
func (dbh *DBHandleImpl)Delete(s DeleteStmt)error {
	if C.delete_db(dbh.dbh,s.CDeleteStmt()) != 0 {
		return errors.New("could not delete rows from table")
	}
	return nil
}