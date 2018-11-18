package ddl

import (
    "testing"
    "strings"
    "errors"
    "generator/layout"
)

func TestDDL(t *testing.T) {
	tc := layout.TestCases()
	for _,c := range *tc {
		if !c.Success {
			continue
		}
		
		l,err := layout.NewLayouterFromReader(strings.NewReader(c.Layout))
		if err != nil {
			t.Fatal(err.Error())
		}

		dbtypes := []string{ "mysql", "postgre" }
		dbconf := &DDLTmplConfig{OutDir: "../../../generated/sql", TmplDir: "../../../tmpl/sql", Recreate: true}
		for _,dbt := range dbtypes {
			var tmpl DDLTmpl = nil
			switch dbt {
			case "mysql":
				tmpl = new(MySQLTmpl)
			case "postgre":
				tmpl = new(PostgreTmpl)
			default:
				t.Fatal(errors.New("invalid database type"))
			}
			if err = Generate(l,tmpl,dbconf); err != nil {
				t.Fatal(err.Error())
			}
		}
	}
}
/*
var simpleDB string = `CREATE DATABASE simpledb1;
CREATE TABLE simpledb1.simpletable1 (
    ID BIGINT NOT NULL AUTO_INCREMENT,
    teststr VARCHAR(255) DEFAULT '',
    testint INT NOT NULL DEFAULT -1,
    testdate TIMESTAMP NOT NULL,
    testfloat DOUBLE NOT NULL DEFAULT 0.0,

    CONSTRAINT PK_simpledb1_simpletable1 PRIMARY KEY(ID)
) ROW_FORMAT=dynamic  ENGINE=innodb ;
`

var complexDB string = `CREATE DATABASE complexdb1;
CREATE TABLE complexdb1.complextable1 (
    ID BIGINT NOT NULL AUTO_INCREMENT,
    teststr VARCHAR(255) DEFAULT '',
    testint INT NOT NULL DEFAULT -1,
    testdate TIMESTAMP NOT NULL,
    testfloat DOUBLE NOT NULL DEFAULT 0.0,

    CONSTRAINT PK_complexdb1_complextable1 PRIMARY KEY(ID)
) ROW_FORMAT=dynamic  ENGINE=innodb ;
CREATE TABLE complexdb1.complextable2 (
    ID BIGINT NOT NULL AUTO_INCREMENT,
    teststr VARCHAR(255) DEFAULT '',
    testint INT NOT NULL DEFAULT -1,
    testdate TIMESTAMP NOT NULL,
    testfloat DOUBLE NOT NULL DEFAULT 0.0,

    CONSTRAINT PK_complexdb1_complextable2 PRIMARY KEY(ID),
    CONSTRAINT UK_complexdb1_complextable2_ID_testint UNIQUE (ID,testint)
) ROW_FORMAT=dynamic  ENGINE=innodb ;

ALTER TABLE complexdb1.complextable2 ADD CONSTRAINT FK_complexdb1_complextable2_ID__REF__complexdb1_complextable1_ID FOREIGN KEY (FK_ID) REFERENCES complexdb1.complextable1 (ID);
`
*/


