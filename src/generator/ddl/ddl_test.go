package ddl

import (
    "testing"
    "strings"
    "os"
    "generator/layout"
)

func TestDDL(t *testing.T) {
	tc := layout.TestCases()
	for _,c := range *tc {
		l,err := layout.NewLayouter(strings.NewReader(c.Layout))
		if err != nil {
			continue
		}
		if err = Generate(l,os.Stdout); err != nil {
			t.Fatal(err.Error())
		}
	}
}

