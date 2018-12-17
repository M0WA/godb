package godb

/*
#include <stdlib.h>

#include <db.h>
#include <column.h>
#include <statements.h>
#include <selectresult.h>
#include <logger.h>

static SelectResult *alloc_select_result() {
	SelectResult *s = calloc(1,sizeof(SelectResult));
	return s;
}

static void free_select_result(SelectResult *res) {
	if(res) {
		free(res);
	}
}

static void go_dump_select_result(SelectResult *res) {
	char *buf = 0;	
	dump_selectresult(res,&buf);
	if(buf) {
		LOGF_DEBUG("%s",buf);
		free(buf);
	}
}

static int get_select_result_colidx_by_name(SelectResult *res,const char *name,unsigned long long *idx) {
	for(size_t i = 0; i < res->ncols; i++) {
		if(strcmp(name,res->cols[i].name) == 0) {
			*idx = i;
			return 0;
		}
	}
	return 1;
}

static void selectresult_free_string(char *str) {
	if(str) {
		free(str);
	}
}
*/
import "C"

import (
	"runtime"
	"errors"
)

type SelectResult interface {
	/*
		Next returns next result row or nil if no (more) rows available
	*/
	Next(DBHandle)(SelectResultRow,error)
	
	/*
		ToNative returns a pointer to this result's C datatype
	*/
	ToNative()*C.SelectResult
}

type SelectResultRow interface {
	GetByName(colname string)interface{}
	GetByIndex(idx uint64)interface{}
}

type SelectResultImpl struct {
	res *C.SelectResult
}

/*
	NewSelectResult allocates a select result
*/
func NewSelectResult()SelectResult {
	r := new(SelectResultImpl)
	r.res = C.alloc_select_result()
	runtime.SetFinalizer(r, FreeSelectResult)
	return r
}

/*
	FreeSelectResult frees all allocated memory for a select result
*/
func FreeSelectResult(res *SelectResultImpl) {
	if res != nil && res.res != nil {
		C.destroy_selectresult(res.res)
		C.free_select_result(res.res)
	}
}

/*
	ToNative implements SelectResult interface
*/
func (s *SelectResultImpl)ToNative()*C.SelectResult {
	return s.res
}

/*
	Next implements SelectResult interface
*/
func (s *SelectResultImpl)Next(dbh DBHandle)(SelectResultRow,error) {
	rc := C.fetch_db(dbh.ToNative(),s.res)
	if rc == 0 {
		return nil,nil
	} else if rc < 0 {
		return nil,errors.New("error while fetching result row")
	} else {
		C.go_dump_select_result(s.res)
		return s,nil
	}
}

/*
	GetByName implements SelectResultRow interface
*/
func (s *SelectResultImpl)GetByName(colname string)interface{} {
	cn := C.CString(colname)
	defer C.selectresult_free_string(cn)
	
	idx := C.ulonglong(0)
	if C.get_select_result_colidx_by_name(s.res,cn,&idx) != 0 {
		return nil
	}
	return s.GetByIndex(uint64(idx))
}

/*
	GetByIndex implements SelectResultRow interface
*/
func (s *SelectResultImpl)GetByIndex(idx uint64)interface{} {
	return nil
}