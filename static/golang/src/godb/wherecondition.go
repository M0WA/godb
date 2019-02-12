package godb
/*
#include <stdlib.h>

#include "where.h"

static WhereCondition *alloc_where_condition() {
	WhereCondition *w = malloc(sizeof(WhereCondition));
	if(!w) {
		return NULL;
	}
	return w;
}

static void free_where_condition(WhereCondition *w) {
	if(w) {
		free(w);
	}
}
*/
import "C"

import (
	"runtime"
)

type WhereCondition interface {
	CWhereCondition()*C.WhereCondition
}

type WhereConditionImpl struct {
	w *C.WhereCondition
}

func freeWhereCondition(w *WhereConditionImpl) {
	C.free_where_condition(w.w)
}

func NewWhereCondition(def DBColumnDef,vals []interface{})(WhereCondition) {
	w := new(WhereConditionImpl)
	w.w = C.alloc_where_condition()
	runtime.SetFinalizer(w, freeWhereCondition)
	return w
}

func (w *WhereConditionImpl)CWhereCondition()*C.WhereCondition {
	return w.w
}
