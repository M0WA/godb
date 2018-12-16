package godb

/*
#include <stdlib.h>

#include <statements.h>

static DeleteStmt *alloc_delete_stmt() {
	DeleteStmt *s = calloc(1,sizeof(DeleteStmt));
	return s;
}

static WhereClause *get_where_clause(DeleteStmt *s) {
	return &s->where;
}
*/
import "C"

import (
	"runtime"
)

type DeleteStmt interface {
	Where()(*C.WhereClause)
	ToNative()(*C.DeleteStmt)
}

type DeleteStmtImpl struct {
	stmt *C.DeleteStmt
}

func NewDeleteStmt()DeleteStmt {
	s := new(DeleteStmtImpl)
	s.stmt = C.alloc_delete_stmt()
	runtime.SetFinalizer(s, FreeDeleteStmt)
	return s
}

func FreeDeleteStmt(s *DeleteStmtImpl) {
	DestroyWhereClause(s.Where())
}

func (stmt *DeleteStmtImpl)Where()(*C.WhereClause) {
	return C.get_where_clause(stmt.ToNative())
}

func (stmt *DeleteStmtImpl)ToNative()(*C.DeleteStmt) {
	return stmt.stmt
}