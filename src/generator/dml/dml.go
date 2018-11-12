package dml

import (
	"generator/layout"
)

type generator interface {
	Generate(l layout.Layouter,out string)error
}

func Generate(l layout.Layouter,out string)error {
	if err := new(cGenerator).Generate(l, out); err != nil {
		return err
	}
	if err := new(golangGenerator).Generate(l, out); err != nil {
		return err
	}
	return nil
}