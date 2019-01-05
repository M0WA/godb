package godb

/*
#include <stdlib.h>

#include "statements.h"

static UpsertStmt* alloc_upsert_stmt() {
	UpsertStmt *s = malloc(sizeof(UpsertStmt));
	if(!s) {
		return NULL;
	}
	return s;
}

static void free_upsert_stmt(UpsertStmt* s) {
	if(s) {
		free(s);
	}
}
*/
import "C"

import (
	"runtime"
)

type UpsertStmt interface {
	CUpsertStmt()*C.UpsertStmt
}

type UpsertStmtImpl struct {
	s *C.UpsertStmt
}

func freeUpsertResult(s *UpsertStmtImpl) {
	C.free_upsert_stmt(s.s)
}

func NewUpsertStmt(tbl DBTable)UpsertStmt {
	r := new(UpsertStmtImpl)
	r.s = C.alloc_upsert_stmt()
	if r.s == nil {
		return nil
	}
	if int(C.create_upsert_stmt(r.s,tbl.CTable())) != 0 {
		return nil
	}
	runtime.SetFinalizer(r, freeUpsertResult)
	return r
}

func (s *UpsertStmtImpl)CUpsertStmt()*C.UpsertStmt {
	return s.s
}