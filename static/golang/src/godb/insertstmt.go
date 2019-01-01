package godb

/*
#include "statements.h"
*/
import "C"

type InsertStmt interface {
	CInsertStmt()*C.InsertStmt
}

type InsertStmtImpl struct {
	s C.InsertStmt
}

func NewInsertStmt(tbl DBTable)InsertStmt {
	r := new(InsertStmtImpl)
	if int(C.create_insert_stmt(&r.s,tbl.CTable())) != 0 {
		return nil
	}
	return r
}

func (s *InsertStmtImpl)CInsertStmt()*C.InsertStmt {
	return &s.s
}