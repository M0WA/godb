package ddl

import (
	"io"
	"fmt"
	"errors"
	"generator/layout"
)

type mysqlGenerator struct {
	recreateDB bool
}

func (g *mysqlGenerator)Database(db *layout.Database, w io.Writer)error {
	sql := "CREATE DATABASE " + db.Name + ";\n"
	if g.recreateDB {
		prefix := 
"CREATE DATABASE IF NOT EXISTS " + db.Name + ";\n" +
"DROP DATABASE " + db.Name + ";\n"
		sql = prefix + sql
	}
	if _,err := io.WriteString(w, sql); err != nil {
		return err
	}
	return nil
}

func (*mysqlGenerator)columntype(d layout.DataType, col *layout.Column, w io.Writer)error {
	typeStr := ""
	switch d {
		case layout.STRING:
			if col.Size == 0 || col.Size > 4294967295 {
				return errors.New(fmt.Sprintf("invalid string column size: %d",col.Size))
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
				return errors.New(fmt.Sprintf("invalid integer column size: %d",col.Size))
			}
		case layout.FLOAT:
			typeStr = "DOUBLE"
		case layout.DATETIME:
			typeStr = "TIMESTAMP"
		default:
			return errors.New("invalid type")	
	}
	if _,err := io.WriteString(w,typeStr); err != nil {
		return err
	}
	return nil
}

func (g *mysqlGenerator)column(db *layout.Database, tbl *layout.Table, col *layout.Column, w io.Writer)error {
	d,err := layout.ParseDataType(col.DataType)
	if err != nil {
		return err
	}
	
	if _,err := io.WriteString(w,"    " + col.Name + " "); err != nil {
		return err
	}
	if err := g.columntype(d, col, w); err != nil {
		return err
	}
	
	if col.NotNull {
		if _,err := io.WriteString(w," NOT NULL"); err != nil {
			return err
		}
	}
	if col.AutoIncrement {
		if _,err := io.WriteString(w," AUTO_INCREMENT"); err != nil {
			return err
		}
	}
	if col.DefaultValue != nil {
		if d == layout.STRING || d == layout.DATETIME {
			if _,err := io.WriteString(w," DEFAULT '" + *col.DefaultValue + "'"); err != nil {
				return err
			}
		} else if _,err := io.WriteString(w," DEFAULT " + *col.DefaultValue); err != nil {
			return err
		}
	}
	
	if _,err := io.WriteString(w,",\n"); err != nil {
		return err
	}
	return nil
}

func (*mysqlGenerator)uniquekeys(db *layout.Database, tbl *layout.Table, w io.Writer)error {
	for _,k := range tbl.UniqueKeys {
		s := ""
		u := db.Name + "_" + tbl.Name
		for i,c := range k.Columns {
			if i > 0 {
				s += ","
			}
			s += c
			u += "_" + c
		}
		if _,err := io.WriteString(w,",\n    CONSTRAINT UK_" + u + " UNIQUE (" + s + ")"); err != nil {
			return err
		}
	}
	return nil
}

func (g *mysqlGenerator)Table(db *layout.Database, tbl *layout.Table, w io.Writer)error {
	if _,err := io.WriteString(w,"CREATE TABLE " + db.Name + "." + tbl.Name + " (\n"); err != nil {
		return err
	}
	for _,col := range tbl.Columns {
		if err := g.column(db,tbl,&col,w); err != nil {
			return err
		}
	}
	
	if _,err := io.WriteString(w,"\n    CONSTRAINT PK_" + db.Name + "_" + tbl.Name + " PRIMARY KEY(" + tbl.PrimaryKey.Column + ")"); err != nil {
		return err
	}
	
	if err := g.uniquekeys(db,tbl,w); err != nil {
		return err
	}
	
	engine := ""
	if tbl.MySQL.Engine != "" {
		engine = " ENGINE=" + tbl.MySQL.Engine + " "
	}
	rowformat := ""
	if tbl.MySQL.RowFormat != "" {
		rowformat = " ROW_FORMAT=" + tbl.MySQL.RowFormat + " "
	}
	
	if _,err := io.WriteString(w,"\n)"+rowformat+engine+";\n"); err != nil {
		return err
	}
	return nil
}

func (*mysqlGenerator)ForeignKey(db *layout.Database, tbl *layout.Table, fk *layout.ForeignKey, w io.Writer)error {
	fkname := "FK_" + db.Name + "_" + tbl.Name + "_" + fk.Column + "__REF__" + fk.MySQL.RefDatabase + "_" + fk.RefTable + "_" + fk.RefColumn
	if _,err := io.WriteString(w,"\nALTER TABLE " + db.Name + "." + tbl.Name + " ADD CONSTRAINT " + fkname + " FOREIGN KEY (FK_ID) REFERENCES " + fk.MySQL.RefDatabase + "." + fk.RefTable + " (" + fk.RefColumn + ");\n"); err != nil {
		return err
	}
	return nil
}
