package godb

import (
    "testing"
    "time"
)

type testTable interface {
	TestInsert(*testing.T,DBHandle)
	TestDelete(*testing.T,DBHandle)
}

type complextable1Test struct { }
type complextable2Test struct { }
type complextable3Test struct { }

func getCredentials()(DBCredentials) {
	creds := NewDBCredentials()
	creds.SetHost("localhost")
	creds.SetPort(3306)
	creds.SetName("complexdb1")
	creds.SetUser("myuser")
	creds.SetPass("mypass")
	return creds
}

func getConnection(t *testing.T,creds DBCredentials, conf DBConfig)(DBHandle) {
	dbh, err := NewDBHandle(conf)
	if err != nil {
		t.Fatal(err.Error())
	}
	if err = dbh.Connect(creds); err != nil {
		t.Fatal(err.Error())
	}
	return dbh
}

func (ct *complextable1Test)TestDelete(t *testing.T, dbh DBHandle) {
	stmt := NewDeleteStmt_complexdb1_complextable1()
	
	vals := []uint16 { 10 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_complexdb1_complextable1_ID(),WhereNotEqual(),ifval)
	
	if err := dbh.Delete(stmt); err != nil {
		t.Fatal(err.Error())
	}
}

func (*complextable1Test)TestInsert(t *testing.T, dbh DBHandle) {
	tbl := New_complexdb1_complextable1()
	tbl.Set_teststr("test")
	tbl.Set_testfloat(11.12)
	tbl.Set_testint(10)
	tbl.Set_testdate(time.Now())
	
	if err := Insert_complexdb1_complextable1(dbh,tbl); err != nil {
		t.Fatal(err.Error())
	}
}

func (*complextable2Test)TestDelete(t *testing.T, dbh DBHandle) {
	stmt := NewDeleteStmt_complexdb1_complextable2()
	
	vals := []uint16 { 10 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_complexdb1_complextable2_ID(),WhereNotEqual(),ifval)
	
	if err := dbh.Delete(stmt); err != nil {
		t.Fatal(err.Error())
	}
}

func (*complextable2Test)TestInsert(t *testing.T, dbh DBHandle) {
	tbl := New_complexdb1_complextable2()
	tbl.Set_teststr("test")
	tbl.Set_testfloat(11.12)
	tbl.Set_testint(10)
	tbl.Set_testdate(time.Now())
	tbl.Set_testfk(10)
	
	if err := Insert_complexdb1_complextable2(dbh,tbl); err != nil {
		t.Fatal(err.Error())
	}
}

func (*complextable3Test)TestDelete(t *testing.T, dbh DBHandle) {
	stmt := NewDeleteStmt_complexdb1_complextable3()
	
	vals := []uint16 { 10 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_complexdb1_complextable3_ID(),WhereNotEqual(),ifval)
	
	if err := dbh.Delete(stmt); err != nil {
		t.Fatal(err.Error())
	}
}

func (*complextable3Test)TestInsert(t *testing.T, dbh DBHandle) {
	tbl := New_complexdb1_complextable3()
	tbl.Set_teststr("test")
	tbl.Set_testfloat(11.12)
	tbl.Set_testint(10)
	tbl.Set_testdate(time.Now())
	tbl.Set_testuniq("30")
	
	if err := Insert_complexdb1_complextable3(dbh,tbl); err != nil {
		t.Fatal(err.Error())
	}
}

func TestGoDB(t *testing.T) {
	SetLogDebug()
	//SetLogFunc()
	
	creds := getCredentials()
	conf,err := NewDBConf(MYSQL)
	if err != nil {
		t.Fatal(err.Error())
	}
	
	dbh := getConnection(t,creds,conf)
	
	tests := []testTable {
		new(complextable1Test),
		new(complextable2Test),
		new(complextable3Test),
	}
	for _,tc := range tests {
		tc.TestDelete(t,dbh)
	}
	for _,tc := range tests {
		tc.TestInsert(t,dbh)
	}
	
	if err = dbh.Disconnect(); err != nil {
		t.Fatal(err.Error())
	}
}