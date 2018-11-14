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

func (*Layout)TypeStrings()map[int]string {
	return typeStrings;
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