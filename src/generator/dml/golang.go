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

func (g *golangTmplData)Imports()string {
	import_time := false; 
	for _,db := range g.L.Databases {
		for _,tb := range db.Tables {
			for _,col := range tb.Columns {
				if col.DataType == "datetime" {
					import_time = true
				}
			}
		}
	}
	
	s := "import ( \n"
	if(import_time) {
		s += "\t\"time\"\n"
	}
	s += ")\n"
	
	return s
}

func (*golangTmplData)CGoType(c *layout.Column)(string) {
	d, err := layout.ParseDataType(c.DataType)
	if err != nil {
		return "invalid"
	}
	switch d {
		case layout.DATETIME:
			return "time.Time" 
		case layout.INT:
			unsigned := ""
			if c.Unsigned {
				unsigned = "u" 
			}
			if c.Size == 16 {
				return "C." + unsigned + "short"
			} else if c.Size == 32 || c.Size == 0 {
				return "C." + unsigned + "long"
			} else if c.Size == 64 {
				return "C." + unsigned + "longlong"
			} else {
				return "invalid"
			}
		case layout.FLOAT:
			return "C.double "
		case layout.STRING:
			return "C.CString"
		default:
			return "invalid"
	}
}

func (*golangTmplData)DataType(c *layout.Column)(string) {
	d, err := layout.ParseDataType(c.DataType)
	if err != nil {
		return "invalid"
	}
	switch d {
		case layout.DATETIME:
			return "time.Time" 
		case layout.INT:
			unsigned := ""
			if c.Unsigned {
				unsigned = "u" 
			}
			if c.Size == 16 {
				return unsigned + "int16"
			} else if c.Size == 32 || c.Size == 0 {
				return unsigned + "int32"
			} else if c.Size == 64 {
				return unsigned + "int64"
			} else {
				return "invalid"
			}
		case layout.FLOAT:
			return "float64"
		case layout.STRING:
			return "string"
		default:
			return "invalid"
	}
}

func (g *golangTmplData)DataVar(c *layout.Column, varname string)(string) {
	return varname + " " + g.DataType(c)
}

func (*golangGenerator)processTemplate(t *golangTmplData,out string,name string,tmpl string)error {
	var err error
	h := template.New(name + ".tmpl")
    h.Funcs(template.FuncMap{"ToUpper": strings.ToUpper})
    h.Funcs(template.FuncMap{"ToLower": strings.ToLower})
    
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
	
	if err := g.processTemplate(t,cd,"tables.go",tmpl + "/golang/src/godb"); err != nil {
		return err
	}
	return nil
}