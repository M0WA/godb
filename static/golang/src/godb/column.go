package godb

/*
#include <column.h>

static DBColumnType get_col_type(DBColumnDef *def) {
	return def->type;
}

static unsigned long long get_col_size(DBColumnDef *def) {
	return def->size;
}

static int is_col_unsigned(DBColumnDef *def) {
	return def->notsigned;
}
*/
import "C"

import (

)

func (def *C.DBColumnDef)GetType()C.DBColumnType {
	return C.get_col_type(def)
}

func (def *C.DBColumnDef)GetSize()C.ulonglong {
	return C.get_col_size(def)
}

func (def *C.DBColumnDef)IsUnsigned()bool {
	if C.is_col_unsigned(def) > 0 {
		return true
	} else {
		return false
	}
}