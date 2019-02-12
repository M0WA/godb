package godb

/*
#include <stdlib.h>

#include "where.h"

static WhereClause *alloc_where_clause() {
	WhereClause *w = malloc(sizeof(WhereClause));
	if(!w) {
		return NULL;
	}
	return w;
}

static int where_append_wherecond(WhereClause *w,WhereCondition *c) {
	return where_append(w,(union _WhereStmt *)c);
}

static int where_append_wherecomp(WhereClause *w,WhereComposite *c) {
	return where_append(w,(union _WhereStmt *)c);
}

static void free_where_clause(WhereClause *w) {
	if(w) {
		where_destroy(w);
		free(w);
	}
}
*/
import "C"

import (
	"runtime"
	"errors"
)

type WhereClause interface {
	AppendComposite(c WhereComposite)error
	AppendCondition(c WhereCondition)error
}

type WhereClauseImpl struct {
	w *C.WhereClause
}

func freeWhereClause(w *WhereClauseImpl) {
	C.free_where_clause(w.w)
}

func NewWhereClause()WhereClause {
	w := new(WhereClauseImpl)
	w.w = C.alloc_where_clause()
	runtime.SetFinalizer(w, freeWhereClause)
	return w
}

func (w *WhereClauseImpl)AppendComposite(c WhereComposite)error {
	if rc := C.where_append_wherecomp(w.w,c.CWhereComposite()); rc != 0 {
		return errors.New("cannot append where composite")
	}
	return nil
}

func (w *WhereClauseImpl)AppendCondition(c WhereCondition)error {
	if rc := C.where_append_wherecond(w.w,c.CWhereCondition()); rc != 0 {
		return errors.New("cannot append where condition")
	}
	return nil
}