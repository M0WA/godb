package godb

/*
#include <stdlib.h>

#include "statements.h"

static UpdateStmt* alloc_update_stmt() {
	UpdateStmt *s = malloc(sizeof(UpdateStmt));
	if(!s) {
		return NULL;
	}
	return s;
}

static void free_update_stmt(UpdateStmt* s) {
	if(s) {
		free(s);
	}
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
	s *C.UpdateStmt
}

func freeUpdateStmt(s *UpdateStmtImpl) {
	C.destroy_update_stmt(s.s)
	C.free_update_stmt(s.s)
}

func NewUpdateStmt(tbl DBTable)UpdateStmt {
	t := new(UpdateStmtImpl)
	t.s = C.alloc_update_stmt()
	if t.s == nil {
		return nil
	}
	if int(C.create_update_stmt(t.s,tbl.CTable())) != 0 {
		return nil
	}
	runtime.SetFinalizer(t, freeUpdateStmt)
	return t
}

func (s *UpdateStmtImpl)CUpdateStmt()*C.UpdateStmt {
	return s.s
}