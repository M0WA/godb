package dml

import (
	"generator/layout"
)

type generator interface {
	Generate(l layout.Layouter,out string, tmpl string)error
}

func Generate(l layout.Layouter,out string, tmpl string)error {
	if err := new(cGenerator).Generate(l, out, tmpl); err != nil {
		return err
	}
	if err := new(golangGenerator).Generate(l, out, tmpl); err != nil {
		return err
	}
	return nil
}