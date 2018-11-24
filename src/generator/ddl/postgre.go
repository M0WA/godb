package ddl

import (
	"os"
	"fmt"
	"strings"
	"generator/layout"
	"text/template"
)

type PostgreTmpl struct {
	
}

func postgreColumnDataType(tbl *layout.Table,col *layout.Column)string {
	d,err := layout.ParseDataType(col.DataType)
	if err != nil {
		fmt.Printf("invalid datatype: %s",col.DataType)
		os.Exit(1)
	}
	if col.Name == tbl.PrimaryKey.Column && d == layout.INT && col.AutoIncrement {
		return "BIGSERIAL"
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
				typeStr = "INTEGER"
			} else if col.Size == 16 {
				typeStr = "SMALLINT"
			} else if col.Size == 64 {
				typeStr = "BIGINT"
			} else {
				fmt.Printf("invalid integer column size: %d",col.Size)
				os.Exit(1)
			}
		case layout.FLOAT:
			typeStr = "DOUBLE PRECISION"
		case layout.DATETIME:
			typeStr = "TIMESTAMP"
		default:
			fmt.Printf("invalid datatype\n")
			os.Exit(1)	
	}
	return typeStr
}

func postgreDBOpts(l layout.Layouter, db string)(out string) {
	switch strings.ToLower(l.Database(db).Charset) {
		case "utf8":
			out += " WITH ENCODING='UTF8'"
		case "":
		default:
			fmt.Printf("invalid charset: %s",l.Database(db).Charset);
			os.Exit(1)
	}
	return
}

func postgreTableOpts(l layout.Layouter, db string, tbl string)string {
	return ""
}

func postgreUseDB(db string)string {
	return "\\c " + db
}

func postgreColSpec(l layout.Layouter, db string, tbl string, col string)string {
	c := l.Column(db, tbl, col)
	t := l.Table(db, tbl)
	
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
	return col + " " + postgreColumnDataType(t,c) + ai + nn + dft
}

func postgreFKSpec(l layout.Layouter, db string, tbl string, fk layout.ForeignKey)string {
	return fk.RefTable + " (" + fk.RefColumn + ")"
}

func postgreIndexSpec(l layout.Layouter, db string, tbl string, k layout.IndexKey)string {
	s := ""
	if k.Postgre.Type != "" {
		s = " USING " + k.Postgre.Type + " "
	}
	return s + "(" + k.Column + " " + k.Postgre.Sort + ")"
}

func (*PostgreTmpl)Funcs()template.FuncMap {
	return template.FuncMap{
		"UseDB": postgreUseDB,
		"DBOpts": postgreDBOpts,
		"TableOpts": postgreTableOpts,
		"ColSpec": postgreColSpec,
		"IndexSpec": postgreIndexSpec,
		"PKSpec": PKSpec,
		"FKSpec": postgreFKSpec,
		"UKSpec": UKSpec,
	}
}

func (*PostgreTmpl)Type()string {
	return "postgre"
}