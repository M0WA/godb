package layout

import (
)

type Database struct {
	Name string
	Tables []Table
	Charset string
	
	MySQL MySQLDB
	Postgre PostgreDB
}
