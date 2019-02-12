package godb

/*

#include "where.h"

*/
import "C"

import (

)

type WhereComposite interface {
	CWhereComposite()*C.WhereComposite
}

type WhereCompositeImpl struct {
	w *C.WhereComposite
}

func (w *WhereCompositeImpl)CWhereComposite()*C.WhereComposite {
	return w.w
}