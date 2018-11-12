package layout

import (

)

type PrimaryKey struct {
	Column string
}

type UniqueKey struct {
	Column []string
}

type ForeignKey struct {
	Column string
	RefColumn string
	
	MySQL MySQLForeignKey 
}
