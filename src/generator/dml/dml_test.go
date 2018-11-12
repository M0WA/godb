package dml

import (
    "testing"
    "strings"
    "generator/layout"
)

func TestC(t *testing.T) {
	tc := layout.TestCases()
	for _,c := range *tc {
		l,err := layout.NewLayouter(strings.NewReader(c.Layout))
		if err != nil {
			continue
		}
		if err = Generate(l,"/home/user/repos/GoDB/tmpl/"); err != nil {
			t.Fatal(err.Error())
		}
	}
}

