package layout

import (
    "testing"
    "strings"
    "reflect"
)

func TestLayout(t *testing.T) {
	for _,c := range tc {
		l,err := NewLayout(strings.NewReader(c.Layout))
		//t.Logf("%v => %v\n",err,l)
		checkResult(t,l,err,c.Result)
	}
}

func TestLayouter(t *testing.T) {
	for _,c := range tc {
		l,err := NewLayouter(strings.NewReader(c.Layout))
		//t.Logf("%v => %v\n",err,l.Layout())
		checkResult(t,l.Layout(),err,c.Result)
		
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

func checkResult(t *testing.T, l *Layout, err error, r *Layout) {
	if l != nil && r == nil {
		//invalid layout was created
		t.Fatal("invalid layout created")
	} else if l == nil && r != nil {
		//check for errors while creating layout
		if err != nil {
			t.Fatal(err.Error())
		} else {
			t.Fatal("unknown error")
		}
	} else if l != nil && r != nil && len(r.Databases) > 0 && !reflect.DeepEqual(*l, *r)  {
		//check if results are equal
		t.Fatalf("layout does not match result \n%v\n!=\n%v",*l,*r)
	}
}


