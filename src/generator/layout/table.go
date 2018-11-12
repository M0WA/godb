package layout

import (
)

type Table struct {
	Name string
	Columns []Column
	PrimaryKey PrimaryKey
	ForeignKeys []ForeignKey
	UniqueKeys []UniqueKey
	
	MySQL MySQLTable
	Postgre PostgreTable
}
