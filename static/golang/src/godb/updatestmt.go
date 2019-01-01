package godb

/*
#include "statements.h"
#include "where.h"

static void free_update_where(UpdateStmt *s) {
	where_destroy(&s->where);
}
*/
import "C"

import (
	"runtime"
)

type UpdateStmt interface {
	CUpdateStmt()*C.UpdateStmt
}

type UpdateStmtImpl struct {
	s C.UpdateStmt
}

func freeUpdateStmt(s *UpdateStmtImpl) {
	C.free_update_where(&s.s)
}

func NewUpdateStmt(tbl DBTable)UpdateStmt {
	t := new(UpdateStmtImpl)
	if int(C.create_update_stmt(&t.s,tbl.CTable())) != 0 {
		return nil
	}
	runtime.SetFinalizer(t, freeUpdateStmt)
	return t
}

func (s *UpdateStmtImpl)CUpdateStmt()*C.UpdateStmt {
	return &s.s
}