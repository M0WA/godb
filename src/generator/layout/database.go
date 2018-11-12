package layout

import (
)

type Database struct {
	Name string
	Tables []Table
	
	MySQL MySQLDB
	Postgre PostgreDB
}
