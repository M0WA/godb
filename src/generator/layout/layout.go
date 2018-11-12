package layout

import (
	"errors"
	"io"
	"io/ioutil"
	"gopkg.in/yaml.v1"
)

type Layout struct {
	Databases []Database
}

func NewLayout(r io.Reader)(*Layout,error) {
	l := new(Layout)
	l.Databases = make([]Database,0)
	
	b := make([]byte,0)
	
	var err error = nil
	if b, err = ioutil.ReadAll(r); err != nil {
	    return nil,err
	}
	if err = yaml.Unmarshal(b, l); err != nil {
	    return nil,err
	}
	if len(l.Databases) == 0 {
		return nil, errors.New("no databases found")
	}
	return l,nil
}

func checkEqualColumn(ld *Database,l *Table,rd *Database,r *Table)bool {
	return false
}

func checkEqualTable(ld *Database,l *Table,rd *Database,r *Table)bool {
	return false
}

func checkEqualDatabase(ll Layouter,l *Database,lr Layouter,r *Database)bool {
	
	//eq := l.Name == r.Name
	
	return false
}

func (l *Layout)Equals(r *Layout)bool {
	for _,ldb := range l.Databases {
		dbEq := false
		for _,rdb := range l.Databases {
			if ldb.Equals(&rdb) {
				dbEq = true
				continue
			}
		}
		if !dbEq {
			return false
		}
	}
	return true
}