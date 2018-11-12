package layout

import (

)

func (l *Database)Equals(r *Database)bool {
	for _,ltbl := range l.Tables {
		tblEq := false
		for _, rtbl := range r.Tables {
			if( ltbl.Equals(&rtbl) ) {
				tblEq = true
				continue
			}
		}
		if( !tblEq ) {
			return false
		}
	}
	return (
	  l.Name == r.Name && 
	  l.MySQL == r.MySQL &&
	  l.Postgre == r.Postgre)
}

type Database struct {
	Name string
	Tables []Table
	
	MySQL MySQLDB
	Postgre PostgreDB
}
