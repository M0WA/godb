package ddl

import (
    "testing"
    "strings"
    "bytes"
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
		
		var s string
		buf := bytes.NewBufferString(s)
		if err = Generate(l,buf); err != nil {
			t.Fatal(err.Error())
		}
		s = buf.String()
		
		switch {
			case l.Database("simpledb1") != nil:
				if s != simpleDB {
					t.Fatalf("invalid sql:\n%s\n\n%s",s,simpleDB)
				}
			case l.Database("complexdb1") != nil:
				if s != complexDB {
					t.Fatalf("invalid sql:\n%s\n\n%s",s,complexDB)
				}
			default:
				t.Fatal("unknown test layout");
		}
	}
}

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



