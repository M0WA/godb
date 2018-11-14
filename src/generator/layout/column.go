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

var typeStrings = map[int]string {
	int(INVALID): "invalid",
	int(STRING): "string",
	int(INT): "int",
	int(FLOAT): "float",
	int(DATETIME): "datetime",
}

func (d DataType)String()(string) {
	if val, ok := typeStrings[int(d)]; ok {
	    return val
	}
	return typeStrings[int(INVALID)]
}

func ParseDataType(d string)(DataType,error) {
	for k,v := range typeStrings {
		if v == d {
			return DataType(k),nil
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
