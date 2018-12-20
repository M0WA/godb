package godb

import (
    "testing"
    "time"
    "fmt"
)

type testTable interface {
	testInsert(*testing.T,DBHandle)
	testDelete(*testing.T,DBHandle)
	testSelect(*testing.T,DBHandle)
}

type complextable1Test struct { }
type complextable2Test struct { }
type complextable3Test struct { }

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

func (ct *complextable1Test)testDelete(t *testing.T, dbh DBHandle) {
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

func (*complextable1Test)testInsert(t *testing.T, dbh DBHandle) {
	tbl := New_complexdb1_complextable1()
	tbl.Set_teststr("test")
	tbl.Set_testfloat(11.12)
	tbl.Set_testint(10)
	tbl.Set_testdate(time.Now())
	
	if err := Insert_complexdb1_complextable1(dbh,tbl); err != nil {
		t.Fatal(err.Error())
	}
}

func (*complextable1Test)testSelect(t *testing.T, dbh DBHandle) {
	stmt := NewSelectStmt_complexdb1_complextable1()
	
	vals := []uint16 { 0 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_complexdb1_complextable1_ID(),WhereNotEqual(),ifval)
	
	res,err := dbh.Select(stmt)
	if err != nil {
		t.Fatal(err.Error())
	}
	
	rows := 0
	for {
		row,err := res.Next(dbh)
		if err != nil {
			t.Fatal(err.Error())
		} else if row == nil {
			break
		}
		
		rowstr := ""
		for idx := range row.Columns() {
			v := row.GetByIndex(uint64(idx))
			if v == nil {
				t.Fatal("invalid column value")
			}
			tse := fmt.Sprintf("%v",v);
			rowstr += " | " + tse
		}
		t.Log(rowstr)
		rows++
	}
	
	if rows != 2 {
		t.Fatal("invalid row count in select")
	}
}

func (*complextable2Test)testDelete(t *testing.T, dbh DBHandle) {
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

func (*complextable2Test)testInsert(t *testing.T, dbh DBHandle) {
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

func (*complextable2Test)testSelect(t *testing.T, dbh DBHandle) {
	stmt := NewSelectStmt_complexdb1_complextable2()
	
	vals := []uint16 { 0 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_complexdb1_complextable2_ID(),WhereNotEqual(),ifval)
	
	res,err := dbh.Select(stmt)
	if err != nil {
		t.Fatal(err.Error())
	}
	
	rows := 0
	for {
		if row,err := res.Next(dbh); err != nil {
			t.Fatal(err.Error())
		} else if row == nil {
			break
		}
		rows++
	}
	
	if rows != 2 {
		t.Fatal("invalid row count in select")
	}
}

func (*complextable3Test)testDelete(t *testing.T, dbh DBHandle) {
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

func (*complextable3Test)testInsert(t *testing.T, dbh DBHandle) {
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

func (*complextable3Test)testSelect(t *testing.T, dbh DBHandle) {
	stmt := NewSelectStmt_complexdb1_complextable3()
	
	vals := []uint16 { 0 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v
	}
	
	stmt.Where().AppendCondition(Def_complexdb1_complextable3_ID(),WhereNotEqual(),ifval)
	
	res,err := dbh.Select(stmt)
	if err != nil {
		t.Fatal(err.Error())
	}
	
	rows := 0
	for {
		if row,err := res.Next(dbh); err != nil {
			t.Fatal(err.Error())
		} else if row == nil {
			break
		}
		rows++
	}
	
	if rows != 2 {
		t.Fatal("invalid row count in select")
	}
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
	
	return confs
}

func TestGoDB(t *testing.T) {
	SetLogDebug()
	//SetLogFunc()
	
	confs := getDBConfigs(t)
	
	for _,cc := range confs {
		dbh := getConnection(t,cc.creds,cc.conf)
		
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
		
		if err := dbh.Disconnect(); err != nil {
			t.Fatal(err.Error())
		}
	}
}