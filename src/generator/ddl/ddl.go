package ddl

import (
	"io"
	"generator/layout"
)

type generator interface {
	Database(*layout.Database, io.Writer)error
	Table(*layout.Database, *layout.Table, io.Writer)error
	ForeignKey(*layout.Database, *layout.Table, *layout.ForeignKey, io.Writer)error
}

func generate(g generator,l layout.Layouter,w io.Writer)error {
	for _,db := range l.Layout().Databases {
		if err := g.Database(&db, w); err != nil {
			return err
		}
		for _,t := range db.Tables {
			if err := g.Table(&db, &t, w); err != nil {
				return err
			}
		}
	}
	
	for _,db := range l.Layout().Databases {
		for _,t := range db.Tables {
			for _,k := range t.ForeignKeys {
				if err := g.ForeignKey(&db, &t, &k, w); err != nil {
					return err
				}
			}	
		}
	}
	return nil
}

func Generate(l layout.Layouter,w io.Writer)error {
	if err := generate(new(mysqlGenerator),l,w); err != nil {
		return err
	}
	if err := generate(new(postgreGenerator),l,w); err != nil {
		return err
	}
	return nil
}