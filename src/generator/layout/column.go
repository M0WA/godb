package layout

import (
	"errors"
)

type DataType int

const (
	INVALID  DataType = iota
	STRING   DataType = iota
	INT	     DataType = iota
	FLOAT    DataType = iota
	DATETIME DataType = iota
)

var typeStrings = map[DataType]string {
	INVALID: "invalid",
	STRING: "string",
	INT: "int",
	FLOAT: "float",
	DATETIME: "datetime",
}

func (d DataType)String()(string) {
	if val, ok := typeStrings[d]; ok {
	    return val
	}
	return typeStrings[INVALID]
}

func ParseDataType(d string)(DataType,error) {
	for k,v := range typeStrings {
		if v == d {
			return k,nil
		}
	}
	return STRING, errors.New("invalid datatype: " + d)
}

func (l *Column)Equals(r *Column)bool {
	return (
	  l.Name == r.Name && 
	  l.DataType == r.DataType &&
	  l.Size == r.Size &&
	  l.NotNull == r.NotNull &&
	  ( (l.DefaultValue == nil && r.DefaultValue == nil) || (*l.DefaultValue == *r.DefaultValue) ) &&
	  l.AutoIncrement == r.AutoIncrement &&
	  l.MySQL == r.MySQL &&
	  l.Postgre == r.Postgre) 
}

type Column struct {
	Name string
	DataType string
	Size uint
	NotNull bool
	DefaultValue *string
	AutoIncrement bool
	
	MySQL MySQLColumn
	Postgre PostgreColumn
}
