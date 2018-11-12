package layout

import (

)

type TestCase struct {
	Layout string
	Result *Layout
}

var tc = []TestCase{
	TestCase{"", &Layout{} },
	TestCase{ oneTableLayout(), &Layout{ Databases: oneTableDatabases() } },
}

func TestCases()*[]TestCase {
	return &tc
}

func oneTableLayout()(string) {
	return `---
databases:
  - name: testdb
    tables:
      - name: testtable
        mysql:
           engine: innodb
           rowformat: dynamic
        primarykey:
          column: ID
        columns:
          - name: ID
            datatype: int
            notnull: true
            autoincrement: true
            size: 64
          - name: teststr
            datatype: string
            notnull: false
            size: 255
            defaultvalue: ""            
          - name: testint
            datatype: int
            notnull: true
            defaultvalue: -1
          - name: testdate
            datatype: datetime
            notnull: true
`
}

func oneTableFields()([]Column) {
	default1 := "-1"
	default2 := ""
	return []Column{
		Column{
			Name: "ID",
			DataType: INT.String(),
			NotNull: true,
			Size: 64,
			AutoIncrement: true,
		},
		Column{
			Name: "teststr",
			DataType: STRING.String(),
			NotNull: false,
			Size: 255,
			DefaultValue: &default2,
		},
		Column{
			Name: "testint",
			DataType: INT.String(),
			NotNull: true,
			DefaultValue: &default1,
		},
		Column{
			Name: "testdate",
			DataType: DATETIME.String(),
			NotNull: true,
		},
	}
}

func oneTableTables()([]Table) {
	return []Table{
		Table{ Name: "testtable", Columns: oneTableFields(), PrimaryKey: PrimaryKey{ Column: "ID" }, MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" } },		
	}
}

func oneTableDatabases()([]Database) {
	return []Database{
		Database{ Name: "testdb", Tables: oneTableTables() },
	}
}