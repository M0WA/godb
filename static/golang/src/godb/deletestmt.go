package godb
/*
#include <stdlib.h>

#include "statements.h"

static DeleteStmt* alloc_delete_stmt() {
	DeleteStmt *s = malloc(sizeof(DeleteStmt));
	if(!s) {
		return NULL;
	}
	return s;
}

static void free_delete_stmt(DeleteStmt* s) {
	if(s) {
		free(s);
	}
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
	s *C.DeleteStmt
}

func freeDeleteStmt(s *DeleteStmtImpl) {
	C.destroy_delete_stmt(s.s)
	C.free_delete_stmt(s.s)
}

func NewDeleteStmt(def DBTableDef)DeleteStmt {
	t := new(DeleteStmtImpl)
	t.s = C.alloc_delete_stmt()
	if t.s == nil {
		return nil
	}
	if int(C.create_delete_stmt(t.s,def.CTableDef())) != 0 {
		return nil
	}
	runtime.SetFinalizer(t, freeDeleteStmt)
	return t
}

func (s *DeleteStmtImpl)CDeleteStmt()*C.DeleteStmt {
	return s.s
}