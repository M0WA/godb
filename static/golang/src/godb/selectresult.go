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

static short get_select_result_short_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	
	return *((short*)res->row[idx]);
}

static long get_select_result_long_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	
	return *((long*)res->row[idx]);
}

static long long get_select_result_longlong_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	
	return *((long*)res->row[idx]);
}

static double get_select_result_float_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	return *((double*)res->row[idx]);
}

static const struct tm* get_select_result_time_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	return (struct tm*)res->row[idx];
}

static const char* get_select_result_string_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	return (const char*)res->row[idx];
}

static const DBColumnDef* get_select_result_coldef_by_colidx(SelectResult *res,unsigned long long idx) {
	if(res->ncols <= idx) {
		return 0; 
	}
	return &res->cols[idx];
}

static unsigned long long get_select_result_col_count(SelectResult *res) {
	return res->ncols;
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
	"time"
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
	Columns()[]*C.DBColumnDef
	GetByName(colname string)interface{}
	GetByIndex(idx uint64)interface{}
	GetByDef(def *C.DBColumnDef)interface{}
}

type SelectResultImpl struct {
	res *C.SelectResult
	rowidx uint64
}

/*
	NewSelectResult allocates a select result
*/
func NewSelectResult()SelectResult {
	r := new(SelectResultImpl)
	r.rowidx = 0
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
		s.rowidx++
		return s,nil
	}
}

/*
	GetByDef implements SelectResultRow interface
*/

func (s *SelectResultImpl)GetByDef(def *C.DBColumnDef)interface{} {
	return s.GetByName(def.GetName())
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
	coldef := C.get_select_result_coldef_by_colidx(s.res,C.ulonglong(idx))
	if coldef == nil {
		return nil
	}
	
	var v interface{} = nil
	switch coldef.GetType() {
		case C.COL_TYPE_STRING:
			v = C.GoString(C.get_select_result_string_by_colidx(s.res,C.ulonglong(idx)))
		case C.COL_TYPE_INT:
			if coldef.GetSize() <= 16 && coldef.GetSize() != 0 {
				v = int16(C.get_select_result_short_by_colidx(s.res,C.ulonglong(idx)))
			} else if coldef.GetSize() <= 32 {
				v = int32(C.get_select_result_long_by_colidx(s.res,C.ulonglong(idx)))
			} else if coldef.GetSize() <= 64 && coldef.GetSize() != 0 {
				v = int64(C.get_select_result_longlong_by_colidx(s.res,C.ulonglong(idx)))
			} else {
				return nil
			}
		case C.COL_TYPE_FLOAT:
			v = float64(C.get_select_result_float_by_colidx(s.res,C.ulonglong(idx)))
		case C.COL_TYPE_DATETIME:
			tmptm := C.get_select_result_time_by_colidx(s.res,C.ulonglong(idx))
			timestamp := C.mktime(tmptm)
			v = time.Unix(int64(timestamp), 0)
		default:
			return nil
	}
	return v
}

func (s *SelectResultImpl)Columns()[]*C.DBColumnDef {
	r := make([]*C.DBColumnDef,0)
	cnt := C.get_select_result_col_count(s.res)
	var i C.ulonglong = 0
	for i = 0 ; i < cnt; i++ {
		r = append(r,C.get_select_result_coldef_by_colidx(s.res,C.ulonglong(i)))
	}
	return r
}
