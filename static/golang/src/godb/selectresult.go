package godb

/*
#include <stdlib.h>

#include "selectresult.h"

static SelectResult* alloc_selectresult() {
	SelectResult *r = malloc(sizeof(SelectResult));
	if(!r) {
		return NULL;
	}
	return r;
}

static void free_selectresult(SelectResult* r) {
	if(r) {
		free(r);
	}
}
*/
import "C"

import (
	"runtime"
)

type SelectResult interface {
	CSelectResult()*C.SelectResult
}

type SelectResultImpl struct {
	stmt SelectStmt
	res *C.SelectResult
}

func freeSelectResult(t *SelectResultImpl) {
	C.destroy_selectresult(t.res)
	C.free_selectresult(t.res)
}

func NewSelectResult(stmt SelectStmt)SelectResult {
	r := new(SelectResultImpl)
	r.res = C.alloc_selectresult()
	if r.res == nil {
		return nil
	}
	if int(C.create_selectresult(stmt.TableDef().CTableDef(),r.res)) != 0 {
		return nil
	}
	runtime.SetFinalizer(r, freeSelectResult)
	return r
}

func (s *SelectResultImpl)CSelectResult()*C.SelectResult {
	return s.res
}