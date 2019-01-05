package godb

import (

)

type Row interface {
	ColumnIndex(colname string)uint64
	ColumnValue(idx uint64)interface{}
}

type RowImpl struct {
	tbl DBTable
	row uint32
}

func (r *RowImpl)ColumnIndex(colname string)uint64 {
	return 0
}

func (r *RowImpl)ColumnValue(idx uint64)interface{} {
	return nil
}
