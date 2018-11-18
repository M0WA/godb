package layout

import (
)

type Table struct {
	Name string
	Columns []Column
	PrimaryKey PrimaryKey
	IndexKeys []IndexKey
	UniqueKeys []UniqueKey
	ForeignKeys []ForeignKey
	
	MySQL MySQLTable
	Postgre PostgreTable
}
