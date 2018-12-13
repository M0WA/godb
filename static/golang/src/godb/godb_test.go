package godb

import (
    "testing"
    "time"
)

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

func testComplexTable1(t *testing.T, dbh DBHandle) {
	
	t.Log("testing complextable1")
	
	tbl := New_complexdb1_complextable1()
	tbl.Set_teststr("test")
	tbl.Set_testfloat(11.12)
	tbl.Set_testint(10)
	tbl.Set_testdate(time.Now())
	
	if err := Insert_complexdb1_complextable1(dbh,tbl); err != nil {
		t.Fatal(err.Error())
	}
	
	t.Log("testing complextable1: done")
}

func TestGoDB(t *testing.T) {
	SetLogFunc()
	SetLogDebug()
	
	creds := getCredentials()
	conf,err := NewDBConf(MYSQL)
	if err != nil {
		t.Fatal(err.Error())
	}
	
	dbh := getConnection(t,creds,conf)
	testComplexTable1(t, dbh)
	
	if err = dbh.Disconnect(); err != nil {
		t.Fatal(err.Error())
	}
}