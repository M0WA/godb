package godb

/*
#include <stdlib.h>

#include "statements.h"

static InsertStmt* alloc_insert_stmt() {
	InsertStmt *s = malloc(sizeof(InsertStmt));
	if(!s) {
		return NULL;
	}
	return s;
}

static void free_insert_stmt(InsertStmt* s) {
	if(s) {
		free(s);
	}
}

*/
import "C"

import (
	"runtime"
)

type InsertStmt interface {
	CInsertStmt()*C.InsertStmt
}

type InsertStmtImpl struct {
	s *C.InsertStmt
}

func freeInsertStmt(s *InsertStmtImpl) {
	if s != nil && s.s != nil {
		C.free_insert_stmt(s.s)
	}
}

func NewInsertStmt(tbl DBTable)InsertStmt {
	r := new(InsertStmtImpl)
	r.s = C.alloc_insert_stmt()
	if r.s == nil {
		return nil
	}
	if int(C.create_insert_stmt(r.s,tbl.CTable())) != 0 {
		return nil
	}
	runtime.SetFinalizer(r, freeInsertStmt)
	return r
}

func (s *InsertStmtImpl)CInsertStmt()*C.InsertStmt {
	return s.s
}