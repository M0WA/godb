package godb

/*
#include "statements.h"
#include "where.h"

static void free_insert_where(SelectStmt *s) {
	where_destroy(&s->where);
}
*/
import "C"

import (
	"runtime"
)

type SelectStmt interface {
	CSelectStmt()*C.SelectStmt
	TableDef()DBTableDef
}

type SelectStmtImpl struct {
	s C.SelectStmt
	def DBTableDef
}

func freeInsertStmt(s *SelectStmtImpl) {
	C.free_insert_where(&s.s)
}

func NewSelectStmt(def DBTableDef)SelectStmt {
	t := new(SelectStmtImpl)
	if int(C.create_select_stmt(&t.s,def.CTableDef())) != 0 {
		return nil
	}
	t.def = def
	runtime.SetFinalizer(t, freeInsertStmt)
	return t
}

func (s *SelectStmtImpl)CSelectStmt()*C.SelectStmt {
	return &s.s
}

func (s *SelectStmtImpl)TableDef()DBTableDef {
	return s.def
}
