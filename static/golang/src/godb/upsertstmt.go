package godb

/*
#include "statements.h"
*/
import "C"

type UpsertStmt interface {
	CUpsertStmt()*C.UpsertStmt
}

type UpsertStmtImpl struct {
	s C.UpsertStmt
}

func NewUpsertStmt(tbl DBTable)UpsertStmt {
	r := new(UpsertStmtImpl)
	if int(C.create_upsert_stmt(&r.s,tbl.CTable())) != 0 {
		return nil
	}
	return r
}

func (s *UpsertStmtImpl)CUpsertStmt()*C.UpsertStmt {
	return &s.s
}