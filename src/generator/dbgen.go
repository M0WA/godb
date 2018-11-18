package main 

import (
    "flag"
    "fmt"
    "os"
	"generator/layout"
	"generator/dml"
	"generator/ddl"
)

const APP_VERSION = "0.1"

// The flag package provides a default help printer via -h switch
var versionFlag *bool = flag.Bool("v", false, "Print the version number.")
var inputFile *string = flag.String("f", "-", "yaml containing database descriptions")
var outputDir *string = flag.String("o", "generated", "output directory for generated sources")
var templateDir *string = flag.String("t", "tmpl", "template directory")
var testLayout *bool = flag.Bool("l", false, "print testlayout and exit")
var disableMySQL *bool = flag.Bool("m", false, "do not generate mysql ddl")
var disablePostgre *bool = flag.Bool("p", false, "do not generate postgre ddl")

func initLayouter()(layout.Layouter) {
	var f *os.File
	var err error
	if *inputFile == "-" {
		fmt.Fprintf(os.Stderr,"reading from stdin\n")
		f = os.Stdin
	} else {
	    f, err = os.Open(*inputFile)
	    defer f.Close()
	}
    if err != nil {
    	fmt.Fprintf(os.Stderr,err.Error()+"\n")
    	return nil
    }
    
    l,err := layout.NewLayouterFromReader(f)
    if err != nil {
    	fmt.Fprintf(os.Stderr,err.Error()+"\n")
    	return nil
    }
    return l
}

func createDDL(l layout.Layouter) {
    dbconf := &ddl.DDLTmplConfig{OutDir: *outputDir + "/sql", TmplDir: *templateDir + "/sql", Recreate: true}
    ddltmpls := make([]ddl.DDLTmpl,0)
    if ! *disableMySQL {
    	ddltmpls = append(ddltmpls,new(ddl.MySQLTmpl))
    }
    if ! *disablePostgre {
    	ddltmpls = append(ddltmpls,new(ddl.PostgreTmpl))
    }
    for _,dd := range ddltmpls {
    	if err := ddl.Generate(l,dd,dbconf); err != nil {
	    	fmt.Fprintf(os.Stderr,err.Error()+"\n")
	    	os.Exit(1)
	    }
    }
}

func createDML(l layout.Layouter) {
    if err := dml.Generate(l,*outputDir,*templateDir); err != nil {
    	fmt.Fprintf(os.Stderr,err.Error()+"\n")
    	os.Exit(1)
    }
}

func main() {
    flag.Parse() // Scan the arguments list 

    if *versionFlag {
        fmt.Println("Version:", APP_VERSION)
    }
    if *testLayout {
        fmt.Print(layout.ComplexLayout("complexdb1"))
        os.Exit(0)
    }
    
    l := initLayouter()
    if l == nil {
    	os.Exit(1)
    }
    
    createDDL(l);
    createDML(l);
}

