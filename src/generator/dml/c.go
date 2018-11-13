package dml

import (
	"os"
	"text/template"
	"generator/layout"
)

type cGenerator struct {
}

type cTmplData struct {
	L *layout.Layout
	ll layout.Layouter
}

func (t *cTmplData)DataType(datatype string)(string) {
	d, err := layout.ParseDataType(datatype)
	if err != nil {
		return "invalid"
	}
	switch d {
		case layout.DATETIME:
			return "struct tm"
		case layout.INT:
			return "long long"
		case layout.FLOAT:
			return "double"
		case layout.STRING:
			return "char*"
		default:
			return "invalid"
	}
}

func (*cGenerator)processTemplate(t *cTmplData,out string,name string,tmpl string)error {
	var err error
	h := template.New(name + ".tmpl")
	if h,err = h.ParseFiles(tmpl + "/" + name + ".tmpl"); err != nil {
		return err
	}
	
	w, err := os.Create(out + "/" + name)
	if err != nil {
		return err
	}
	if err = h.Execute(w,t); err != nil {
		return err
	}
	return nil
}

func (g *cGenerator)Generate(l layout.Layouter,out string, tmpl string)error {
	hd := out + "/c/include"
	cd := out + "/c/src"
	os.MkdirAll(hd, 0700)
	os.MkdirAll(cd, 0700)
	
	t := new(cTmplData)
	t.L = l.Layout()
	t.ll = l
	
	if err := g.processTemplate(t,hd,"tables.h",tmpl + "/c"); err != nil {
		return err
	}
	if err := g.processTemplate(t,cd,"tables.c",tmpl + "/c"); err != nil {
		return err
	}
	return nil
}