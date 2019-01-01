package godb

/*
#include "table.h"
#include "column.h"

const char* get_dbname_by_dbtabledef(const DBTableDef *def) {
	return def->database;
}

const char* get_tblname_by_dbtabledef(const DBTableDef *def) {
	return def->name;
}

const DBColumnDef* get_coldefs_by_dbtabledef(const DBTableDef *def,unsigned long long idx) {
	return &(def->cols[idx]);
}

unsigned long long get_colcnt_by_dbtabledef(const DBTableDef *def) {
	return def->ncols;
}
*/
import "C"

type DBTableDef interface {
	CTableDef()*C.DBTableDef
	DatabaseName()*string
	TableName()*string
	ColumnDefs()*[]DBColumnDef
}

type DBTableDefImpl struct {
	def *C.DBTableDef
}

func NewDBTableDef(def *C.DBTableDef)DBTableDef {
	d := new(DBTableDefImpl)
	d.def = def
	return d
}

func (d *DBTableDefImpl)CTableDef()*C.DBTableDef {
	return d.def
}

func (d *DBTableDefImpl)DatabaseName()*string {
	r := C.GoString(C.get_dbname_by_dbtabledef(d.def))
	return &r
}

func (d *DBTableDefImpl)TableName()*string{
	r := C.GoString(C.get_tblname_by_dbtabledef(d.def))
	return &r
}

func (d *DBTableDefImpl)ColumnDefs()*[]DBColumnDef{
	cnt := uint64(C.get_colcnt_by_dbtabledef(d.def))
	defs := make([]DBColumnDef,0)
	var i uint64
	for i = 0; i < cnt; i++ {
		defs = append(defs,NewDBColumnDef(C.get_coldefs_by_dbtabledef(d.def,C.ulonglong(cnt))))
	}
	return &defs
}
