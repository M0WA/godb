package layout

import (
	"errors"
	"io"
	"fmt"
	"io/ioutil"
	"crypto/sha512"
	"gopkg.in/yaml.v1"
	"encoding/hex"
)

type Layout struct {
	Databases []Database
	CheckSum string
}

func (*Layout)TypeStrings()map[int]string {
	return typeStrings;
}

func (l *Layout)setCheckSum() {
	d,_ := yaml.Marshal(l)
	c := sha512.New()
	c.Write([]byte(d))
	l.CheckSum = hex.EncodeToString(c.Sum(nil))
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
	l.setCheckSum()
	return l,nil
}