package layout

import (

)

type MySQLIndexKey struct {
	Sort string
	Type string
}

type PostgreIndexKey struct {
	Sort string
	Type string
}

type IndexKey struct {
	Column string
	
	MySQL MySQLIndexKey
	Postgre PostgreIndexKey
}

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
	RefDatabase string
}