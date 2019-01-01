package godb

/*
#include "statements.h"
#include "where.h"

static void free_delete_where(DeleteStmt *s) {
	where_destroy(&s->where);
}
*/
import "C"

import (
	"runtime"
)

type DeleteStmt interface {
	CDeleteStmt()*C.DeleteStmt
}

type DeleteStmtImpl struct {
	s C.DeleteStmt
}

func freeDeleteStmt(s *DeleteStmtImpl) {
	C.free_delete_where(&s.s)
}

func NewDeleteStmt(def DBTableDef)DeleteStmt {
	t := new(DeleteStmtImpl)
	if int(C.create_delete_stmt(&t.s,def.CTableDef())) != 0 {
		return nil
	}
	runtime.SetFinalizer(t, freeInsertStmt)
	return t
}

func (s *DeleteStmtImpl)CDeleteStmt()*C.DeleteStmt {
	return &s.s
}