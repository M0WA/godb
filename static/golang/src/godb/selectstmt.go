package godb

/*
#include <stdlib.h>

#include <statements.h>
#include <selectresult.h>

static SelectStmt *alloc_select_stmt() {
	SelectStmt *s = calloc(1,sizeof(SelectStmt));
	return s;
}

static WhereClause *get_where_clause_selectstmt(SelectStmt *s) {
	return &s->where;
}
*/
import "C"

import (
	"runtime"
)

type SelectStmt interface {
	Where()*C.WhereClause
	ToNative()*C.SelectStmt
}

type SelectStmtImpl struct {
	stmt *C.SelectStmt
}

/*
	NewSelectStmt allocates a select statement
*/
func NewSelectStmt()SelectStmt {
	s := new(SelectStmtImpl)
	s.stmt = C.alloc_select_stmt()
	runtime.SetFinalizer(s, FreeSelectStmt)
	return s
}

/*
	FreeSelectStmt frees all allocated memory for a delete statement
*/
func FreeSelectStmt(s *SelectStmtImpl) {
	DestroyWhereClause(s.Where())
}

/*
	Where implements SelectStmt interface
*/
func (s *SelectStmtImpl)Where()*C.WhereClause {
	return C.get_where_clause_selectstmt(s.ToNative())
}

/*
	ToNative implements SelectStmt interface
*/
func (s *SelectStmtImpl)ToNative()*C.SelectStmt {
	return s.stmt
}