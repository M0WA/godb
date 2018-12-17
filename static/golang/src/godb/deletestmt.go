package godb

/*
#include <stdlib.h>
#include <statements.h>

static DeleteStmt *alloc_delete_stmt() {
	DeleteStmt *s = calloc(1,sizeof(DeleteStmt));
	return s;
}

static WhereClause *get_where_clause_deletestmt(DeleteStmt *s) {
	return &s->where;
}
*/
import "C"

import (
	"runtime"
)

type DeleteStmt interface {
	/*
		Where returns the where clause part of the statement
	*/
	Where()*C.WhereClause
	
	/*
		ToNative returns a pointer to this statement's C datatype
	*/
	ToNative()*C.DeleteStmt
}

type DeleteStmtImpl struct {
	stmt *C.DeleteStmt
}

/*
	NewDeleteStmt allocates a delete statement
*/
func NewDeleteStmt()DeleteStmt {
	s := new(DeleteStmtImpl)
	s.stmt = C.alloc_delete_stmt()
	runtime.SetFinalizer(s, FreeDeleteStmt)
	return s
}

/*
	FreeDeleteStmt frees all allocated memory for a delete statement
*/
func FreeDeleteStmt(s *DeleteStmtImpl) {
	DestroyWhereClause(s.Where())
}

/*
	Where implements DeleteStmt interface
*/
func (stmt *DeleteStmtImpl)Where()*C.WhereClause {
	return C.get_where_clause_deletestmt(stmt.ToNative())
}

/*
	ToNative implements DeleteStmt interface
*/
func (stmt *DeleteStmtImpl)ToNative()*C.DeleteStmt {
	return stmt.stmt
}