package ddl

import (
	"io"
	"generator/layout"
)

type postgreGenerator struct {
	
}

func (*postgreGenerator)Database(*layout.Database, io.Writer)error {
	return nil
}

func (*postgreGenerator)Table(*layout.Database, *layout.Table, io.Writer)error {
	return nil
}

func (*postgreGenerator)ForeignKey(*layout.Database, *layout.Table, *layout.ForeignKey, io.Writer)error {
	return nil
}