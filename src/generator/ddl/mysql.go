package ddl

import (
	"os"
	"fmt"
	"strings"
	"text/template"
	"generator/layout"
)

type MySQLTmpl struct {
	
}

func mysqlColumnDataType(col *layout.Column)string {
	d,err := layout.ParseDataType(col.DataType)
	if err != nil {
		fmt.Printf("invalid datatype: %s",col.DataType)
		os.Exit(1)
	}
	typeStr := ""
	switch d {
		case layout.STRING:
			if col.Size == 0 || col.Size > 4294967295 {
				fmt.Printf("invalid string column size: %d\n",col.Size)
				os.Exit(1)
			} else {
				typeStr = fmt.Sprintf("VARCHAR(%d)",col.Size)
			}
		case layout.INT:
			if col.Size == 32 || col.Size == 0 {
				typeStr = "INT"
			} else if col.Size == 16 {
				typeStr = "SMALLINT"
			} else if col.Size == 64 {
				typeStr = "BIGINT"
			} else {
				fmt.Printf("invalid integer column size: %d",col.Size)
				os.Exit(1)
			}
		case layout.FLOAT:
			typeStr = "DOUBLE"
		case layout.DATETIME:
			typeStr = "TIMESTAMP"
		default:
			fmt.Printf("invalid datatype\n")
			os.Exit(1)	
	}
	return typeStr
}

func mysqlDBOpts(l layout.Layouter, db string)(out string) {
	switch strings.ToLower(l.Database(db).Charset) {
		case "utf8":
			out += " CHARSET=utf_mb4"
		case "latin1":
			out += " CHARSET=latin1"
		case "":
		default:
			fmt.Printf("invalid charset: %s",l.Database(db).Charset);
			os.Exit(1)
	}
	
	if l.Database(db).MySQL.Collation != "" {
		out += " COLLATION=" + l.Database(db).MySQL.Collation
	}
	return
}

func mysqlTableOpts(l layout.Layouter, db string, tbl string)(out string) {
	t := l.Table(db, tbl)
	if t.MySQL.Engine != "" {
		out += " ENGINE=" + t.MySQL.Engine
	}
	if t.MySQL.RowFormat != "" {
		out += " ROW_FORMAT=" + t.MySQL.RowFormat
	}
	if t.MySQL.DefaultCharset != "" {
		out += " CHARSET=" + t.MySQL.DefaultCharset
	}
	return
}

func mysqlUseDB(db string)string {
	return "use " + db + ";"
}

func mysqlColSpec(l layout.Layouter, db string, tbl string, col string)string {
	c := l.Column(db, tbl, col)
	
	d,err := layout.ParseDataType(c.DataType)
	if err != nil {
		fmt.Printf("invalid datatype: %s",c.DataType)
		os.Exit(1)
	}
	
	nn := ""
	if c.NotNull {
		nn = " NOT NULL"
	}
	dft := ""
	if c.DefaultValue != nil {
		if *c.DefaultValue != "NULL" && (d == layout.STRING || d == layout.DATETIME) {
			dft = " DEFAULT '" + *c.DefaultValue + "'"
		} else {
			dft = " DEFAULT " + *c.DefaultValue
		}
	}
	ai := ""
	if c.AutoIncrement {
		ai = " AUTO_INCREMENT "
	}
	return col + " " + mysqlColumnDataType(c) + ai + nn + dft
}

func mysqlFKSpec(l layout.Layouter, db string, tbl string, fk layout.ForeignKey)string {
	ref := fk.RefTable
	if fk.MySQL.RefDatabase != "" {
		ref = fk.MySQL.RefDatabase + "." + fk.RefTable
	}
	return ref + " (" + fk.Column + ")"
}

func mysqlIndexSpec(l layout.Layouter, db string, tbl string, k layout.IndexKey)string {
	s := ""
	if k.MySQL.Type != "" {
		s = " USING " + k.MySQL.Type
	}
	return "(" + k.Column + " "+k.MySQL.Sort+")" + s
}

func (*MySQLTmpl)Funcs()template.FuncMap {
	return template.FuncMap{
		"UseDB": mysqlUseDB,
		"DBOpts": mysqlDBOpts,
		"TableOpts": mysqlTableOpts,
		"ColSpec": mysqlColSpec,
		"IndexSpec": mysqlIndexSpec,
		"PKSpec": PKSpec,
		"FKSpec": mysqlFKSpec,
		"UKSpec": UKSpec,
	}
}

func (*MySQLTmpl)Type()string {
	return "mysql"
}