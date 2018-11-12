package layout

import (
    "testing"
    "strings"
    "reflect"
)

func TestLayout(t *testing.T) {
	for _,c := range tc {
		l,err := NewLayout(strings.NewReader(c.Layout))
		if err != nil && l != nil {
			t.Fatal("layout with errors is non-nil")
		} else if c.Success && err != nil {
			t.Fatal(err.Error())
		} else if !c.Success && err == nil {
			t.Fatalf("test did not fail as expected: %v",l)
		}
		compareResult(t,l,c.Result)
	}
}

func TestLayouter(t *testing.T) {
	for _,c := range tc {
		if !c.Success {
			continue
		}
		
		l,err := NewLayouterFromReader(strings.NewReader(c.Layout))
		if err != nil {
			t.Fatal(err.Error())
		}
		
		for _,db := range l.Layout().Databases {
			if l.Database(db.Name) == nil {
				t.Fatalf("could not find database: %s",db.Name)
			}
			for _,tbl := range db.Tables {
				if l.Table(db.Name, tbl.Name) == nil {
					t.Fatalf("could not find table: %s.%s",db.Name,tbl.Name)
				}
				for _,col := range tbl.Columns {
					if l.Column(db.Name, tbl.Name,col.Name) == nil {
						t.Fatalf("could not find column: %s.%s.%s",db.Name,tbl.Name,col.Name)
					}
				}
			}
		}
	}
}

func compareResult(t *testing.T, l *Layout, r *Layout) {
	if l == nil && r == nil {
		return
	} else if l != nil && r == nil {
		t.Fatal("layout does not match nil-result")
	} else if l == nil && r != nil {
		t.Fatal("nil-layout does not match result")
	} else if !reflect.DeepEqual(l,r) {
		t.Fatalf("layout does not match result_\n%v\n\n%v\n",l,r)
	}
}
