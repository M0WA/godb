package layout

import (

)

type MySQLDB struct {
	Charset string
	Collation string
}

type MySQLTable struct {
	Engine string
	RowFormat string
	DefaultCharset string
}

type MySQLColumn struct {
}

type MySQLForeignKey struct {
	RefDatabase string
}