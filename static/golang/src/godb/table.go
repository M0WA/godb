package godb

/*
#include "table.h"
*/
import "C"
import (
	"runtime"
)

type DBRow interface {
	ColumnIndex(colname string)uint64
	ColumnValue(idx uint64)interface{}
}

type DBTable interface {
	CTable()*C.DBTable
	TableDef()DBTableDef
	TableRow(row uint32)DBRow
}

type DBTableImpl struct {
	dbt C.DBTable
	def DBTableDef
}

func freeDBTable(t DBTable) {
	C.destroy_dbtable(t.CTable())
}

func NewDBTable(def DBTableDef, rows uint32)DBTable {
	t := new(DBTableImpl)
	t.def = def
	C.create_dbtable(&t.dbt,def.CTableDef(),C.ulong(rows))
	runtime.SetFinalizer(t, freeDBTable)
	return t
}

func (t *DBTableImpl)CTable()*C.DBTable {
	return &t.dbt
}

func (t *DBTableImpl)TableDef()DBTableDef {
	return t.def
}

func (t *DBTableImpl)TableRow(row uint32)DBRow {
	return nil
}
