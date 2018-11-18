package ddl

import (
	"os"
	"text/template"
	"generator/layout"
)

type DDLTmplConfig struct {
	OutDir string 
	TmplDir string
	Recreate bool
}

type DDLTmplData struct {
	Layouter layout.Layouter
	Recreate bool
}

type DDLTmpl interface {
	Funcs()template.FuncMap
	Type()string
}

func Generate(l layout.Layouter,tmpl DDLTmpl,conf *DDLTmplConfig)error {
	tmpls := []string{ "01_databases.sql", "02_tables.sql", "03_indexes.sql", "04_foreignkeys.sql" }
	ddl := new(DDLTmplData)
	ddl.Layouter = l
	ddl.Recreate = conf.Recreate
	for _,name := range tmpls {
		if err := ProcessTemplate(name,ddl,tmpl,conf); err != nil {
			return err
		}
	}
	return nil
}

func ProcessTemplate(name string,t *DDLTmplData,tmpl DDLTmpl,conf *DDLTmplConfig)error {
	os.MkdirAll(conf.OutDir + "/" + tmpl.Type(), 0700)
	
	var err error
	h := template.New(name + ".tmpl")
    h.Funcs(tmpl.Funcs())
    
	if h,err = h.ParseFiles(conf.TmplDir + "/" + name + ".tmpl"); err != nil {
		return err
	}
	
	w, err := os.Create(conf.OutDir + "/" + tmpl.Type() + "/" + name)
	if err != nil {
		return err
	}
	if err = h.Execute(w,t); err != nil {
		return err
	}
	return nil
}
