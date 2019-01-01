package godb

/*
#include "selectresult.h"
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
	res C.SelectResult
}

func freeSelectResult(t *SelectResultImpl) {
	C.destroy_selectresult(&t.res)
}

func NewSelectResult(stmt SelectStmt)SelectResult {
	r := new(SelectResultImpl)
	if int(C.create_selectresult(stmt.TableDef().CTableDef(),&r.res)) != 0 {
		return nil
	}
	runtime.SetFinalizer(r, freeSelectResult)
	return r
}

func (s *SelectResultImpl)CSelectResult()*C.SelectResult {
	return &s.res
}