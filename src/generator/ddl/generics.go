package ddl

import (
	"strings"
	"generator/layout"
)

func UKSpec(l layout.Layouter, db string, table string, uk layout.UniqueKey)string {
	cols := ""
	name := ""
	for i,k := range uk.Columns {
		if i != 0 {
			cols += ","
			name += "_"
		}
		cols += k
		name += strings.ToUpper(k)
	}
	return "CONSTRAINT UK_" + strings.ToUpper(db) + "_" + strings.ToUpper(table) + "_" + name + " UNIQUE KEY(" + cols + ")"
}

func PKSpec(l layout.Layouter, db string, table string, pk layout.PrimaryKey)string {
	  return "CONSTRAINT PK_" + strings.ToUpper(db) + "_" + strings.ToUpper(table) + " PRIMARY KEY(" + pk.Column + ")"
}
