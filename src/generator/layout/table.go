package layout

import (
	"reflect"
)

func (l *Table)Equals(r *Table)bool {
	for _,lcol := range l.Columns {
		colEq := false
		for _, rcol := range r.Columns {
			if( lcol.Equals(&rcol) ) {
				colEq = true
				continue
			}
		}
		if( !colEq ) {
			return false
		}
	}
	return (
	  l.Name == r.Name && 
	  l.PrimaryKey == r.PrimaryKey &&
	  reflect.DeepEqual(l.ForeignKeys, r.ForeignKeys) &&
	  reflect.DeepEqual(l.UniqueKeys,r.UniqueKeys) &&
	  l.MySQL == r.MySQL &&
	  l.Postgre == r.Postgre)
}

type Table struct {
	Name string
	Columns []Column
	PrimaryKey PrimaryKey
	ForeignKeys []ForeignKey
	UniqueKeys []UniqueKey
	
	MySQL MySQLTable
	Postgre PostgreTable
}
