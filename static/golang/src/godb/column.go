package godb

/*
#include "column.h"
*/
import "C"

import (
)

type DBColumnDef interface {
	CColumnDef()*C.DBColumnDef
}

type DBColumnDefImpl struct {
	def *C.DBColumnDef
}

func NewDBColumnDef(def *C.DBColumnDef)DBColumnDef {
	d := new(DBColumnDefImpl)
	d.def = def
	return d
}

func (d *DBColumnDefImpl)CColumnDef()*C.DBColumnDef {
	return d.def
}