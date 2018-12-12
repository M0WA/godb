package godb

// #include <table.h>
import "C"

import (

)

type DBTableDef interface {
	Definition()*C.DBTableDef
}