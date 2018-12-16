package godb

/*
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <where.h>
#include <column.h>

void free_where_condition(WhereCondition *c);
void free_where_composite(WhereComposite *c);
void free_where_clause(WhereClause *c);

static WhereCondition *alloc_where_condition(const struct _DBColumnDef *col,unsigned long long vals) {
	WhereCondition *c = calloc(1,sizeof(WhereCondition));
	c->type = WHERE_COND;	
	c->values =	calloc(vals,sizeof(void*));
	for(size_t i = 0; i < vals; i++) {
		c->values[i] = malloc(get_column_bufsize(col));
	}
	c->cnt = vals;
	return c;
}

void free_where_condition(WhereCondition *c) {
	if(c) {
		for(size_t i = 0; i < c->cnt; i++) {
			if(c->values[i]) {
				free((void*)c->values[i]);
			}
		}
		if(c->values) {
			free(c->values);
		}
		free(c);
		c = 0;
	}
}

static int append_where_condition(WhereClause *clause, WhereCondition *cond) {	
	return where_append(clause,(WhereStmt*)cond);
}

static int append_condition_value_string(WhereCondition *cond, unsigned long long idx, const char *val) {
	strcpy((void*)cond->values[idx],val);
	return 0;
}

static int append_condition_value_float(WhereCondition *cond, unsigned long long idx, double val) {
	*((double*)cond->values[idx]) = val;
	return 0;
}

static int append_condition_value_short(WhereCondition *cond, unsigned long long idx, short val) {
	*((short*)cond->values[idx]) = val;
	return 0;
}

static int append_condition_value_long(WhereCondition *cond, unsigned long long idx, long val) {
	*((long*)cond->values[idx]) = val;
	return 0;
}

static int append_condition_value_longlong(WhereCondition *cond, unsigned long long idx, long long val) {
	*((long long*)cond->values[idx]) = val;
	return 0;
}

static int append_condition_value_datetime(WhereCondition *cond, unsigned long long idx, struct tm *val) {
	memcpy((void*)cond->values[idx],val,sizeof(struct tm));
	return 0;
}

static int append_where_composite(WhereClause *clause, WhereComposite *comp) {	
	return where_append(clause,(WhereStmt*)comp);
}

static WhereComposite *alloc_where_composite() {
	WhereComposite *c = calloc(1,sizeof(WhereComposite));
	c->type = WHERE_COMP;
	return c;
}

void free_where_composite(WhereComposite *c) {
	if(c) {
		free(c);
		c = 0;
	}
}

static WhereClause *alloc_where_clause() {
	WhereClause *c = calloc(1,sizeof(WhereClause));
	return c;
}

void free_where_clause(WhereClause *c) {
	if(c) {
		where_destroy_free(c,free_where_condition);
		free(c);
		c = 0;
	}
}
*/
import "C"

import (
	"runtime"
	"errors"
	"time"
)

type WhereClause interface {
	AppendComposite(*C.WhereComposite)error
	AppendCondition(*C.DBColumnDef,C.WhereCondOperator,[]interface{})error
}

type WhereComposite interface {
	AppendClause(*C.WhereClause)error
}

func (wc *C.WhereClause)AppendComposite(comp *C.WhereComposite)error {
	rc := C.append_where_composite(wc,comp)
	if rc != 0 {
		return errors.New("cannot append composite")
	}
	return nil
}

func createCondition(col *C.DBColumnDef,op C.WhereCondOperator, v []interface{})(*C.WhereCondition, error) {
	c := C.alloc_where_condition(col,C.ulonglong(len(v)))
	c.cond = op
	c.def = col
	
	for idx,i := range v {
		switch col.GetType() {
		case C.COL_TYPE_STRING:
			str := C.CString(i.(string))
			C.append_condition_value_string(c,C.ulonglong(idx),str)
		case C.COL_TYPE_INT:
			if col.GetSize() <= 16 && col.GetSize() != 0 {
				if col.IsUnsigned() {
					C.append_condition_value_short(c,C.ulonglong(idx),C.short(i.(uint16)))
				} else {
					C.append_condition_value_short(c,C.ulonglong(idx),C.short(i.(int16)))
				}
			} else if col.GetSize() <= 32 {
				if col.IsUnsigned() {
					C.append_condition_value_long(c,C.ulonglong(idx),C.long(i.(uint32)))
				} else {
					C.append_condition_value_long(c,C.ulonglong(idx),C.long(i.(int32)))
				}
			} else if col.GetSize() <= 64 && col.GetSize() != 0 {
				if col.IsUnsigned() {
					C.append_condition_value_longlong(c,C.ulonglong(idx),C.longlong(i.(uint64)))
				} else {
					C.append_condition_value_longlong(c,C.ulonglong(idx),C.longlong(i.(int64)))
				}
			} else {
				return nil, errors.New("invalid int type")
			}
		case C.COL_TYPE_FLOAT:
			val := C.double(i.(float64))
			C.append_condition_value_float(c,C.ulonglong(idx),val);
		case C.COL_TYPE_DATETIME:
			tmp := C.long(i.(time.Time).Unix())
			tm := new(C.struct_tm)
			tm = C.gmtime_r(&tmp, tm)
			C.append_condition_value_datetime(c,C.ulonglong(idx),tm)
		default:
			return nil, errors.New("invalid data type")
		}
	}
	return c,nil
}

func (wc *C.WhereClause)AppendCondition(col *C.DBColumnDef,op C.WhereCondOperator, v []interface{})error {
	c, err := createCondition(col,op, v)
	if err != nil {
		return err
	}
	
	rc := C.append_where_condition(wc,c)
	if rc != 0 {
		return errors.New("cannot append condition")
	}
	return nil
}

func (comp *C.WhereComposite)AppendClause(clause *C.WhereClause)error {
	rc := C.where_comp_append(comp,clause)
	if rc != 0 {
		return errors.New("cannot append clause")
	}
	return nil
}

func NewWhereComposite()WhereComposite {
	c := C.alloc_where_composite()
	return c
}

func NewWhereClause()WhereClause {
	c := C.alloc_where_clause()
	runtime.SetFinalizer(c, DestroyWhereClause)
	return c
}

func DestroyWhereClause(clause *C.WhereClause) {
	C.free_where_clause(clause)
}

func WhereNotEqual()C.WhereCondOperator {
	return C.WHERE_NOT_EQUAL
}

func WhereLike()C.WhereCondOperator {
	return C.WHERE_LIKE
}

func WhereNotLike()C.WhereCondOperator {
	return C.WHERE_NOT_LIKE
}

func WhereIsNull()C.WhereCondOperator {
	return C.WHERE_IS_NULL
}

func WhereIsNotNull()C.WhereCondOperator {
	return C.WHERE_IS_NOT_NULL
}

func WhereAnd()C.WhereCompOperator {
	return C.WHERE_AND
}

func WhereOr()C.WhereCompOperator {
	return C.WHERE_OR
}
