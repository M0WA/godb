package godb

import (
    "testing"
    "time"
)

func getMySQLCredentials()(DBCredentials) {
	creds := NewDBCredentials()
	creds.SetHost("localhost")
	creds.SetPort(3306)
	creds.SetName("complexdb1")
	creds.SetUser("myuser")
	creds.SetPass("mypass")
	return creds
}

func getPostgresCredentials()(DBCredentials) {
	creds := NewDBCredentials()
	creds.SetHost("localhost")
	creds.SetPort(5432)
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

type ConfigCreds struct {
	conf DBConfig
	creds DBCredentials
}

func getDBConfigs(t *testing.T)map[DBType]ConfigCreds {
	confs := make(map[DBType]ConfigCreds,0)
	
	mysql,err := NewDBConfig(MYSQL)
	if err != nil {
		t.Fatal(err.Error())
	}
	confs[MYSQL] = ConfigCreds{ mysql, getMySQLCredentials() }
	
	postgres,err := NewDBConfig(POSTGRES)
	if err != nil {
		t.Fatal(err.Error())
	}
	confs[POSTGRES] = ConfigCreds{ postgres, getPostgresCredentials() }

	dbimysqldbi,err := NewDBConfig(DBI_MYSQL)
	if err != nil {
		t.Fatal(err.Error())
	}
	confs[DBI_MYSQL] = ConfigCreds{ dbimysqldbi, getMySQLCredentials() }

	dbipostgres,err := NewDBConfig(DBI_POSTGRES)
	if err != nil {
		t.Fatal(err.Error())
	}
	confs[DBI_POSTGRES] = ConfigCreds{ dbipostgres, getPostgresCredentials() }

	return confs
}

func testInsert(t *testing.T,dbh DBHandle) {
	tbl := NewComplexdb1_Complextable3(1)
	tbl.SetTestint(33,0)
	tbl.SetTeststr("test",0)
	tbl.SetTestfloat(10.10,0)
	tbl.SetTestdate(time.Now(),0)
	tbl.SetTestuniq("testuniq",0)
	
	s := tbl.InsertStmt()
	if dbh.Insert(s) != nil {
		t.Fatal("error while insert");
	}
}

func testUpsert(t *testing.T,dbh DBHandle) {
	tbl := NewComplexdb1_Complextable3(1)
	tbl.SetTestint(33,0)
	tbl.SetTeststr("test",0)
	tbl.SetTestfloat(12.12,0)
	tbl.SetTestdate(time.Now(),0)
	tbl.SetTestuniq("testuniq",0)
	
	s := tbl.UpsertStmt()
	if dbh.Upsert(s) != nil {
		t.Fatal("error while upsert");
	}
}

func testDelete(t *testing.T,dbh DBHandle) {
	//TODO: add way to access table def without instanciating an DBTable object
	//def  := NewComplexdb1_Complextable3(1).TableDef()
	//TODO: add possibility to add where columns
	//stmt := NewDeleteStmt(def)	
}

func TestGoDB(t *testing.T) {
	SetLogDebug()
	//SetLogFunc()
	
	confs := getDBConfigs(t)
	
	for _,cc := range confs {
		dbh := getConnection(t,cc.creds,cc.conf)
		testDelete(t,dbh)
		testInsert(t,dbh)
		testUpsert(t,dbh)
		/*
		tests := []testTable {
			new(Complextable1Test),
			new(complextable2Test),
			new(complextable3Test),
		}
		for _,tc := range tests {
			tc.testDelete(t,dbh)
			tc.testInsert(t,dbh)
			tc.testSelect(t,dbh)
		}
		*/
		
		if err := dbh.Disconnect(); err != nil {
			t.Fatal(err.Error())
		}
	}
}