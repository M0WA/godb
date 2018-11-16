package dml

import (
	"os"
	"fmt"
	"strings"
	"text/template"
	"generator/layout"
)

type cGenerator struct {
}

type cTmplData struct {
	L *layout.Layout
	ll layout.Layouter
	TypeStrings map[int]string;
}

func (*cTmplData)DataVar(c *layout.Column, varname string)(string) {
	d, err := layout.ParseDataType(c.DataType)
	if err != nil {
		return "invalid"
	}
	switch d {
		case layout.DATETIME:
			return "struct tm " + varname 
		case layout.INT:
			return "long long " + varname
		case layout.FLOAT:
			return "double " + varname
		case layout.STRING:
			return fmt.Sprintf("char %s[%d]",varname, c.Size + 1);
		default:
			return "invalid"
	}
}

func (*cTmplData)PtrVar(c *layout.Column, varname string)(string) {
	d, err := layout.ParseDataType(c.DataType)
	if err != nil {
		return "invalid"
	}
	switch d {
		case layout.DATETIME:
			return "struct tm* " + varname 
		case layout.INT:
			return "long long* " + varname
		case layout.FLOAT:
			return "double* " + varname
		case layout.STRING:
			return "const char* " + varname;
		default:
			return "invalid"
	}
}

func (*cTmplData)SizeOf(c *layout.Column)(string) {
	d, err := layout.ParseDataType(c.DataType)
	if err != nil {
		return "invalid"
	}
	s := ""
	switch d {
		case layout.DATETIME:
			s = "struct tm"
		case layout.INT:
			s = "long long"
		case layout.FLOAT:
			s = "double"
		case layout.STRING:
			return fmt.Sprintf("(sizeof(char) * %d)", c.Size + 1);
		default:
			s = "invalid"
	}
	
	return fmt.Sprintf("sizeof(%s)",s)
}

func (*cGenerator)processTemplate(t *cTmplData,out string,name string,tmpl string)error {
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

func (g *cGenerator)Generate(l layout.Layouter,out string, tmpl string)error {
	hd := out + "/c/include"
	cd := out + "/c/src"
	os.MkdirAll(hd, 0700)
	os.MkdirAll(cd, 0700)
	
	t := new(cTmplData)
	t.L = l.Layout()
	t.ll = l
	
	if err := g.processTemplate(t,hd,"columntypes.h",tmpl + "/c"); err != nil {
		return err
	}
	if err := g.processTemplate(t,hd,"names.h",tmpl + "/c"); err != nil {
		return err
	}
	if err := g.processTemplate(t,hd,"tables.h",tmpl + "/c"); err != nil {
		return err
	}
	if err := g.processTemplate(t,cd,"tables.c",tmpl + "/c"); err != nil {
		return err
	}
	if err := g.processTemplate(t,hd,"tests.h",tmpl + "/c"); err != nil {
		return err
	}
	if err := g.processTemplate(t,cd,"tests.c",tmpl + "/c"); err != nil {
		return err
	}
	return nil
}