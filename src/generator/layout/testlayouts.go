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
            notnull: false
            autoincrement: true
            size: 64
            unsigned: true
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
			NotNull: false,
			Size: 64,
			AutoIncrement: true,
			Unsigned: true,
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
          - name: testfk
            datatype: int
            notnull: false
            autoincrement: false
            size: 64
            defaultvalue: "0"
            unsigned: true
        foreignkeys:
          - column: testfk
            refcolumn: ID
            reftable: complextable1
            mysql:
              refdatabase: ` + db + `
        uniquekeys:
          - columns:
              - ID
              - testint
        indexkeys:
          - column: testfloat
          - column: teststr
`
	t3 := singleOneTableLayout("complextable3") + `
          - name: testuniq
            datatype: string
            notnull: false
            autoincrement: false
            size: 64
            defaultvalue: ""
            unsigned: true
        uniquekeys:
          - columns:
              - testuniq
`

	return `---
databases:
  - name: ` + db + `
    tables:`+ t1 + t2 +t3
}

func complexTables(db string)([]Table) {
	t1 := Table{ Name: "complextable1", Columns: oneTableFields(), PrimaryKey: PrimaryKey{ Column: "ID" }, MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" } }

	f := ForeignKey{
		Column: "testfk",
		RefColumn: "ID",
		RefTable: "complextable1",
		MySQL: MySQLForeignKey{ RefDatabase: db  },
	}
	u := UniqueKey{
		Columns: []string{"ID","testint"},
	}
	
	idxs := make([]IndexKey,0)
	idxs = append(idxs,IndexKey{Column: "testfloat"})
	idxs = append(idxs,IndexKey{Column: "teststr"})

	dfk := "0"
	c := oneTableFields()
	c = append(c, Column{
			Name: "testfk",
			DataType: INT.String(),
			NotNull: false,
			Size: 64,
			AutoIncrement: false,
			DefaultValue: &dfk,
			Unsigned: true,
	})
	t2 := Table{
		Name: "complextable2",
		Columns: c,
		PrimaryKey: PrimaryKey{ Column: "ID" },
		ForeignKeys: []ForeignKey{ f },
		UniqueKeys: []UniqueKey{ u },
		IndexKeys: idxs,
		MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" },
	}

	u2 := UniqueKey{
		Columns: []string{"testuniq"},
	}
	duk2 := ""
	c2 := oneTableFields()
	c2 = append(c2, Column{
			Name: "testuniq",
			DataType: STRING.String(),
			NotNull: false,
			Size: 64,
			AutoIncrement: false,
			DefaultValue: &duk2,
			Unsigned: true,
	})
	t3 := Table{
		Name: "complextable3",
		Columns: c2,
		PrimaryKey: PrimaryKey{ Column: "ID" },
		UniqueKeys: []UniqueKey{ u2 },
		MySQL: MySQLTable{ Engine: "innodb", RowFormat: "dynamic" },
	}
	return []Table{ t1, t2, t3 }
}
