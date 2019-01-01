package godb

import (
    "testing"
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

	confs[DBI_MYSQL] = ConfigCreds{ mysql, getMySQLCredentials() }
	confs[DBI_POSTGRES] = ConfigCreds{ postgres, getPostgresCredentials() }

	return confs
}

func TestGoDB(t *testing.T) {
	SetLogDebug()
	//SetLogFunc()
	
	confs := getDBConfigs(t)
	
	for _,cc := range confs {
		dbh := getConnection(t,cc.creds,cc.conf)
		
		/*
		tests := []testTable {
			new(complextable1Test),
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