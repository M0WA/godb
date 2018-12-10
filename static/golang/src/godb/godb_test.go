package godb

import (
    "testing"
)

func getCredentials()(DBCredentials) {
	var creds DBCredentials = NewDBCredentials()
	creds.SetHost("localhost")
	creds.SetPort(3306)
	creds.SetName("mydb")
	creds.SetUser("myuser")
	creds.SetPass("mypass")
	return creds
}

func getConnection(t *testing.T,creds DBCredentials, conf DBConfig)(DBHandler) {
	dbh, err := NewDBHandle(conf)
	if err != nil {
		t.Fatal(err.Error())
	}
	if err = dbh.Connect(creds); err != nil {
		t.Fatal(err.Error())
	}
	return dbh
}

func TestGoDB(t *testing.T) {
	var creds DBCredentials = getCredentials()
	conf,err := NewDBConf(MYSQL)
	if err != nil {
		t.Fatal(err.Error())
	}
	
	dbh := getConnection(t,creds,conf)
	
	if err = dbh.Disconnect(); err != nil {
		t.Fatal(err.Error())
	}
}