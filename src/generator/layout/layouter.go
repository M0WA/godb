package layout

import (
	"io"
)

type Layouter interface {
	Database(db string)(*Database)
	Table(db string, tbl string)(*Table)
	Column(db string, tbl string, col string)(*Column)
	Layout()(*Layout)
}

type tblIdx struct {
	idx int
	col map[string]int
}

type dbIdx struct {
	idx int
	tbl map[string]tblIdx
}

type layouterImpl struct {
	layout *Layout
	db map[string]dbIdx
}

func (l *layouterImpl)initLookup() {
	l.db = make(map[string]dbIdx,0)
	for idb,db := range l.layout.Databases {
		l.db[db.Name] = dbIdx{ idb, make(map[string]tblIdx,0) }
		for itb,tb := range db.Tables {
			l.db[db.Name].tbl[tb.Name] = tblIdx{ itb, make(map[string]int,0) }
			for icol,col := range tb.Columns {
				l.db[db.Name].tbl[tb.Name].col[col.Name] = icol
			}
		}
	}
}

func (l *layouterImpl)init()(error) {
	l.initLookup()
	return nil
}

func (l *layouterImpl)Layout()(*Layout) {
	return l.layout
}

func (l *layouterImpl)Database(db string)(*Database) {
	if _, ok := l.db[db]; !ok {
		return nil
	}
	dbidx := l.db[db].idx
	return &(l.layout.Databases[dbidx])
}

func (l *layouterImpl)Table(db string, tbl string)(*Table) {
	if _, ok := l.db[db]; !ok {
		return nil
	}
	if _, ok := l.db[db].tbl[tbl]; !ok {
		return nil
	}
	dbidx  := l.db[db].idx
	tblidx := l.db[db].tbl[tbl].idx
	return &(l.layout.Databases[dbidx].Tables[tblidx])
}

func (l *layouterImpl)Column(db string, tbl string, col string)(*Column) {
	if _, ok := l.db[db]; !ok {
		return nil
	}
	if _, ok := l.db[db].tbl[tbl]; !ok {
		return nil
	}
	if _, ok := l.db[db].tbl[tbl].col[col]; !ok {
		return nil
	}
	dbidx  := l.db[db].idx
	tblidx := l.db[db].tbl[tbl].idx
	colidx := l.db[db].tbl[tbl].col[col]
	return &(l.layout.Databases[dbidx].Tables[tblidx].Columns[colidx])
}

func NewLayouter(l *Layout)(Layouter,error) {
	ll := new(layouterImpl)
	ll.layout = l
	if err := ll.init(); err != nil {
		return nil, err
	}
	
	return ll,nil
}

func NewLayouterFromReader(r io.Reader)(Layouter,error) {
	var err error
	var layout *Layout
	if layout, err = NewLayout(r); err != nil {
		return nil, err
	}
	return NewLayouter(layout)
}