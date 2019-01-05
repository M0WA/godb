package godb

/*
#include <stdlib.h>

#include "table.h"

static DBTable* alloc_dbtable() {
	DBTable *t = malloc(sizeof(DBTable));
	if(!t) {
		return NULL;
	}
	return t;
}

static void free_dbtable(DBTable *t) {
	if(t) {
		free(t);
	}
}
*/
import "C"
import (
	"runtime"
)

type DBTable interface {
	CTable()*C.DBTable
	TableDef()DBTableDef
	Row(row uint32)Row
}

type DBTableImpl struct {
	dbt *C.DBTable
	def DBTableDef
}

func freeDBTable(t DBTable) {
	C.destroy_dbtable(t.CTable())
	C.free_dbtable(t.CTable())
}

func NewDBTable(def DBTableDef, rows uint32)DBTable {
	t := new(DBTableImpl)
	t.def = def
	t.dbt = C.alloc_dbtable()
	C.create_dbtable(t.dbt,def.CTableDef(),C.ulong(rows))
	runtime.SetFinalizer(t, freeDBTable)
	return t
}

func NewDBTableFromCDBTable(def DBTableDef,tbl *C.DBTable)DBTable {
	t := new(DBTableImpl)
	t.def = def
	t.dbt = tbl
	return t
}

func (t *DBTableImpl)CTable()*C.DBTable {
	return t.dbt
}

func (t *DBTableImpl)TableDef()DBTableDef {
	return t.def
}

func (t *DBTableImpl)Row(row uint32)Row {
	r := new(RowImpl)
	r.tbl = t
	r.row = row
	return r
}
