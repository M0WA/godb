package layout

import (
)

type TestCase struct {
	Layout string
	Result *Layout
	Success bool
}

var tc = []TestCase{
	TestCase{ Layout: "", Result: nil, Success: false },
	TestCase{ Layout: OneTableLayout("simpledb1"), Result: &Layout{ Databases: oneTableDatabases("simpledb1") }, Success: true },
	TestCase{ Layout: ComplexLayout("complexdb1"), Result: &Layout{ Databases: complexDatabases("complexdb1") }, Success: true },
}

func TestCases()*[]TestCase {
	return &tc
}

/* **************************** one table layout testcase ********************************* */

func singleOneTableLayout(tbl string)string{
	return `
      - name: `+tbl+`
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
          - name: testfloat
            datatype: float
            notnull: true
            defaultvalue: 0.0 `
}

func OneTableLayout(db string)(string) {
	return `---
databases:
  - name: ` + db + `
    tables:`+
    singleOneTableLayout("simpletable1")
}

func oneTableFields()([]Column) {
	default1 := "-1"
	default2 := ""
	default3 := "0.0"
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
		Column{
			Name: "testfloat",
			DataType: FLOAT.String(),
			NotNull: true,
			DefaultValue: &default3,
		},
	}
}

func oneTableTables()([]Table) {
	return []Table{
		Table{ Name: "simpletable1", Columns: oneTableFields(), PrimaryKey: PrimaryKey{ Column: "ID" }, MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" } },		
	}
}

func oneTableDatabases(db string)([]Database) {
	return []Database{
		Database{ Name: db, Tables: oneTableTables() },
	}
}

/* **************************** complex table layout testcase ********************************* */

func complexDatabases(db string)([]Database) {
	return []Database{
		Database{ Name: db, Tables: complexTables(db) },
	}
}

func ComplexLayout(db string)(string) {
	t1 := singleOneTableLayout("complextable1")
	t2 := singleOneTableLayout("complextable2") + `
        foreignkeys:
          - column: ID
            refcolumn: ID
            reftable: complextable1
            mysql:
              refdatabase: ` + db + `
        uniquekeys:
          - columns:
              - ID
              - testint
`
	return `---
databases:
  - name: ` + db + `
    tables:`+ t1 + t2
}

func complexTables(db string)([]Table) {
	t1 := Table{ Name: "complextable1", Columns: oneTableFields(), PrimaryKey: PrimaryKey{ Column: "ID" }, MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" } }
	
	f := ForeignKey{
		Column: "ID",
		RefColumn: "ID",
		RefTable: "complextable1",
		MySQL: MySQLForeignKey{ RefDatabase: db  },
	}
	u := UniqueKey{
		Columns: []string{"ID","testint"},
	}
	t2 := Table{ 
		Name: "complextable2", 
		Columns: oneTableFields(), 
		PrimaryKey: PrimaryKey{ Column: "ID" },
		ForeignKeys: []ForeignKey{ f },
		UniqueKeys: []UniqueKey{ u },
		MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" }, 
	}
	return []Table{ t1, t2 }
}
