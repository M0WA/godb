package dml

import (
	_ "os"
	"bytes"
	"text/template"
	"generator/layout"
)

type cGenerator struct {
}

func (*cGenerator)Generate(l layout.Layouter,out string)error {
	var s string
	//buf := os.Stdout
	buf := bytes.NewBufferString(s)
	
	var err error
	h := template.New("table.h.tmpl")
	if h,err = h.ParseFiles(out + "/c/table.h.tmpl"); err != nil {
		return err
	}
	if err = h.Execute(buf,l.Layout()); err != nil {
		return err
	}
	
	c := template.New("table.c.tmpl")
	if c,err = c.ParseFiles(out + "/c/table.c.tmpl"); err != nil {
		return err
	}
	if err = c.Execute(buf,l.Layout()); err != nil {
		return err
	}
	
	return nil
}