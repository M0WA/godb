package dml

import (
	"os"
	"strings"
	"text/template"
	"generator/layout"
)

type golangGenerator struct {
	
}

type golangTmplData struct {
	L *layout.Layout
	ll layout.Layouter
}

func (*golangGenerator)processTemplate(t *golangTmplData,out string,name string,tmpl string)error {
	var err error
	h := template.New(name + ".tmpl")
    h.Funcs(template.FuncMap{"ToUpper": strings.ToUpper})
    
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

func (g *golangGenerator)Generate(l layout.Layouter,out string, tmpl string)error {
	cd := out + "/golang/src/godb"
	os.MkdirAll(cd, 0700)
	
	t := new(golangTmplData)
	t.L = l.Layout()
	t.ll = l
	
	if err := g.processTemplate(t,cd,"tableinterfaces.go",tmpl + "/golang/src/godb"); err != nil {
		return err
	}
	return nil
}