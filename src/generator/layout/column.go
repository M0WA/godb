package layout

import (
	"errors"
)

type DataType int

const (
	STRING   DataType = iota
	INT	     DataType = iota
	FLOAT    DataType = iota
	DATETIME DataType = iota
)

var typeStrings = map[DataType]string {
	STRING: "string",
	INT: "int",
	FLOAT: "float",
	DATETIME: "datetime",
}

func (d DataType)String()(string) {
	if val, ok := typeStrings[d]; ok {
	    return val
	}
	return "unknown"
}

func ParseDataType(d string)(DataType,error) {
	for k,v := range typeStrings {
		if v == d {
			return k,nil
		}
	}
	return STRING, errors.New("invalid datatype: " + d)
}

type Column struct {
	Name string
	DataType string
	Type DataType
	Size uint
	NotNull bool
	DefaultValue *string
	AutoIncrement bool
	
	MySQL MySQLColumn
	PostgreSQL PostgreSQLColumn
}
