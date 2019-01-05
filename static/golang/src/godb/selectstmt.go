package godb

/*
#include <stdlib.h>

#include "statements.h"

static SelectStmt* alloc_select_stmt() {
	SelectStmt *s = malloc(sizeof(SelectStmt));
	if(!s) {
		return NULL;
	}
	return s;
}

static void free_select_stmt(SelectStmt* s) {
	if(s) {
		free(s);
	}
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
	s *C.SelectStmt
	def DBTableDef
}

func freeSelectStmt(s *SelectStmtImpl) {
	C.destroy_select_stmt(s.s)
	C.free_select_stmt(s.s)
}

func NewSelectStmt(def DBTableDef)SelectStmt {
	t := new(SelectStmtImpl)
	t.s = C.alloc_select_stmt()
	if t.s == nil {
		return nil
	}
	if int(C.create_select_stmt(t.s,def.CTableDef())) != 0 {
		return nil
	}
	t.def = def
	runtime.SetFinalizer(t, freeSelectStmt)
	return t
}

func (s *SelectStmtImpl)CSelectStmt()*C.SelectStmt {
	return s.s
}

func (s *SelectStmtImpl)TableDef()DBTableDef {
	return s.def
}
