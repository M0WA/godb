package layout

import (

)

type PrimaryKey struct {
	Column string
}

type UniqueKey struct {
	Columns []string
}

type ForeignKey struct {
	Column string
	RefColumn string
	RefTable string
	
	MySQL MySQLForeignKey 
}
