# GoDB - database framework

GoDB's main goal is to abstract code from your database manufacturer while giving a strong binding to the actual database structure.
It is written with a strong consideration of limited resources like memory and cpu power to be "big-data ready".
Think of it as an advanced version of the well known libdbi with the main difference that you do not have to write __any__ SQL in your code.
All SQL is generated for you by the GoDB framework. 

For more information visit the GoDB [homepage](https://go-db.net) and the offical [github](https://github.com/M0WA/GoDB) page.

## Table of contents
1. [Features](#Features)
2. [Usage/Install](#Usage)
    1. [Install](#Install)
    2. [Example/Test](#Example)
3. [Components](#Components)
    1. [Code Generator](#CodeGenerator)
    2. [Library](#Library)
4. [API](#API)
    1. [YAML Format](#YamlFormat)
    2. [C](#CLib)
    3. [Golang](#Golang)
5. [FAQ](#FAQ)
    1. [I found a bug, missing a feature or have a patch](#bugs)
    2. [Missing brand-specific features](#brandfeatures)


## Features <a name="Features"></a>

<table cellspacing="15"><tr><td style="vertical-align:top">
Databases:<br><br>

<table border=1>
<tr><td>Manufacturer</td><td>DBI</td><td>NATIVE</td></tr>
<tr><td>MySQL</td><td>yes</td><td>yes</td></tr>
<tr><td>PostgreSQL</td><td>yes</td><td>yes</td></tr>
<tr><td>Oracle DB</td><td>TBD</td><td>TBD</td></tr>
<tr><td>IBM DB2</td><td>TBD</td><td>TBD</td></tr>
</table>
</td><td style="vertical-align:top">
Languages:<br><br>

<table border=1>
<tr><td>Languages</td><td>State</td></tr>
<tr><td>C</td><td>yes</td></tr>
<tr><td>C++</td><td>TBD</td></tr>
<tr><td>Golang</td><td>TBD</td></tr>
<tr><td>Python</td><td>TBD</td></tr>
</table>
</td><td style="vertical-align:top">
Statements:<br><br>

<table border=1>
<tr><td>Type</td><td>State</td></tr>
<tr><td>Select</td><td>yes</td></tr>
<tr><td>Insert</td><td>yes</td></tr>
<tr><td>Update</td><td>yes</td></tr>
<tr><td>Delete</td><td>yes</td></tr>
<tr><td>Upsert</td><td>yes</td></tr>
<tr><td>Joins</td><td>TBD</td></tr>
<tr><td>Group/Sum</td><td>TBD</td></tr>
</table>
</td><td>
</td></tr></table>

* Prepared Statements (optional,where applicable)
* Maximum Brand Independance
* Full Language Flexibility
* Extensive Test Framework
* Minimal Memory + CPU Footprint
* Open Source
* Type Safety
* Business Support Available

## Usage <a name="Usage"></a>

### Install <a name="Install"></a>

get and build the example library:

	git clone https://github.com/M0WA/GoDB
	cd GoDB
	cp db.yaml.example db.yaml
	source goenv
	DATABASE_YAML=db.yaml make
	
### Example <a name="Example"></a>

initialize the example databases for tests:

	# build the generator
	make generator

	# generate example sql/code
	bin/./generator -l | bin/./generator -o generated -t tmpl

	# initialization of the example database
	scripts/./init_db.sh -U myuser

reset the example database manually (without user initialization):

	# for mysql databases
	cat generated/sql/mysql/* | sudo mysql -u root

    # for postgresql databases
    cat generated/sql/postgre/* | sudo -u postgres psql -U postgres

run all tests:

	make test

test library for memory leaks/memory access errors etc:

	make valgrind
	
## Components <a name="Components"></a>

The GoDB framework consists of 2 components. A code generator responsible for generating SQL and language code
for your database structure. The second component is one or more language specific library to access data.

### Code Generator <a name="Code Generator"></a>

The code generator is written in Golang and follows it's standard build rules:

	cd GoDB
	source goenv
	go build generator/layout
	go build generator/dml
	go build generator/ddl

then to build the application itself:

	go build -o bin/generator generator

For a reference have a look at Makefile located at the root of this repository.

### Language Libraries <a name="Library"></a>

The other components are language specific libaries that are independently documented
in <a href="#API>API</a>.

## API <a name="API"></a>

This section gives an overview of the available configuration settings for the [generator](#YamlFormat)
or language specific APIs.

### YAML Format <a name="YamlFormat"></a>

the following is a full-featured example database structure

	---
	databases:
	  - name: complexdb1
	    tables:
	      - name: complextable1
	        mysql:
	           engine: innodb
	           rowformat: dynamic
	        primarykey:
	          column: ID
	        columns:
	          - name: ID
	            datatype: int
	            notnull: false
	            autoincrement: true
	            size: 64
	            unsigned: true
	          - name: teststr
	            datatype: string
	            notnull: false
	            size: 255
	            defaultvalue: ""
	          - name: testint
	            datatype: int
	            notnull: true
	            defaultvalue: -1
	          - name: testdate
	            datatype: datetime
	            notnull: true
	          - name: testfloat
	            datatype: float
	            notnull: true
	            defaultvalue: 0.0 
	      - name: complextable2
	        mysql:
	           engine: innodb
	           rowformat: dynamic
	        primarykey:
	          column: ID
	        columns:
	          - name: ID
	            datatype: int
	            notnull: false
	            autoincrement: true
	            size: 64
	            unsigned: true
	          - name: teststr
	            datatype: string
	            notnull: false
	            size: 255
	            defaultvalue: ""
	          - name: testint
	            datatype: int
	            notnull: true
	            defaultvalue: -1
	          - name: testdate
	            datatype: datetime
	            notnull: true
	          - name: testfloat
	            datatype: float
	            notnull: true
	            defaultvalue: 0.0 
	          - name: testfk
	            datatype: int
	            notnull: false
	            autoincrement: false
	            size: 64
	            defaultvalue: "0"
	            unsigned: true
	        foreignkeys:
	          - column: testfk
	            refcolumn: ID
	            reftable: complextable1
	            mysql:
	              refdatabase: complexdb1
	        uniquekeys:
	          - columns:
	              - ID
	              - testint


### C <a name="CLib"></a>

connect to mysql database:

	// initialize library
	init_dblib();
	
	// settings
	DBConfig config = (DBConfig){
		.type = DB_TYPE_MYSQL,
		.mysql.autoreconnect = 1,
		.mysql.compression = 1,
		.mysql.preparedstatements = 1,
	};
	
	// create a database handle
	DBHandle *dbh = create_dbhandle(&config);
	if ( !dbh ) {
		LOG_FATAL(1,"create_dbhandle() failed"); }
	
	// set credentials
	DBCredentials creds = () {
		.host = "localhost",
		.port = 3306,
		.name = "complexdb1",
		.user = "myuser",
		.pass = "mypass",
	};
	
	// connect to database
	if( connect_db(dbh,&creds) ) {
		LOG_FATAL(1,"connect_db() failed"); }
	
	// -> do some work here <-
	
	// disconnect from database
	if ( disconnect_db(dbh) ) {
		LOG_FATAL(1,"disconnect_db() failed"); }
	
	// destroy the database handle
	if( destroy_dbhandle(dbh) ) {
		LOG_FATAL(1,"destroy_dbhandle() failed"); }
	
	// shutdown library
	exit_dblib();

insert rows into exampledb.exampletable:

	// create tables to insert
	TABLE_STRUCT(exampledb,exampletable,tbl);
	set_example_values(&tbl);
	
	TABLE_STRUCT(exampledb,exampletable,tbl2);
	set_example_values(&tbl2);
	
	// single row insert
	if( INSERT_ONE_DBTABLE(dbh,&tbl.dbtbl) ) {
		LOG_FATAL(1,"insert failed"); }
	
	// bulk insert (multiple rows)
	DBTable* rows[2] = { &(tbl.dbtbl), &(tbl2.dbtbl) };
	const DBTable *const*const rowp = (const DBTable *const*const)&rows;
	if( insert_dbtable(dbh,rowp,2) ) {
		LOG_FATAL(1,"bulk insert failed"); }

select rows from exampledb.exampletable include where clause:
	
	SelectStmt stmt;
	selectstmt_exampledb_exampletable(&stmt);
	
	char test[] = "test";
	
	struct _WhereCondition cond;
	memset(&cond,0,sizeof(struct _WhereCondition));
	cond.cond = WHERE_EQUAL;
	cond.type = WHERE_COND;
	cond.def = &(stmt->defs[1]);
	cond.values = (const void*[]){&test};
	cond.cnt = 1;
	
	if( where_append(&stmt.where,(union _WhereStmt *)&cond) ) {
		LOG_FATAL(1,"exampledb.exampletable: cannot append where stmt"); }
	
	SelectResult res;
	memset(&res,0,sizeof(res));
	if( select_db(dbh,&stmt,&res) ) {
		LOG_FATAL(1,"exampledb.exampletable: error while select"); }
	
	int rc = 0;
	size_t rowcount = 0;
	while( (rc = fetch_db(dbh,&res)) > 0){
		rowcount++;	
	}
	LOGF_DEBUG("fetched %lu rows",rowcount);
	if(rc != 0 || rowcount == 0) {
		LOGF_FATAL(1,"exampledb.exampletable: error while fetch: rows: %lu, rc: %d",rowcount,rc); }
	
	DESTROY_STMT( (&stmt) );
	destroy_selectresult(&res);

update rows from exampledb.exampletable:
	
	TABLE_STRUCT(exampledb,exampletable,tbl);
	
	int id = 10;

	struct _WhereCondition cond;
	memset(&cond,0,sizeof(struct _WhereCondition));
	cond.cond = WHERE_NOT_EQUAL;
	cond.type = WHERE_COND;
	cond.def = &(tbl.dbtbl.def->cols[0]);
	cond.values = (const void*[]){&id};
	cond.cnt = 1;
	
	double testfloat = 11.11;
	UpdateStmt stmt;
	memset(&stmt,0,sizeof(UpdateStmt));
	stmt.defs = &(tbl.dbtbl.def->cols[4]);
	stmt.ncols = 1;
	stmt.valbuf = (const void*[]){&testfloat};
	
	if( where_append(&stmt.where,(union _WhereStmt *)&cond) ) {
		LOG_FATAL(1,"exampledb,exampletable: cannot append where stmt"); }
		
	if(update_db(dbh,&stmt)) {
		LOG_FATAL(1,"exampledb,exampletable: error while update"); }
	
	DESTROY_STMT(&stmt);

delete rows from exampledb.exampletable including where clause:

	DeleteStmt stmt;
	deletestmt_exampledb_exampletable(&stmt);
	
	int id = 10;
	
	struct _WhereCondition cond;
	memset(&cond,0,sizeof(struct _WhereCondition));
	cond.cond = WHERE_NOT_EQUAL;
	cond.type = WHERE_COND;
	cond.def = &(stmt.def->cols[0]);
	cond.values = (const void*[]){&id};
	cond.cnt = 1;
	
	if( where_append(&stmt.where,(union _WhereStmt *)&cond) ) {
		LOG_FATAL(1,"exampledb.exampletable: cannot append where stmt"); }
	
	if(delete_db(dbh,&stmt)) {
		LOG_FATAL(1,"exampledb.exampletable: error while delete"); }
	
	DESTROY_STMT(&stmt);

### Golang <a name="Golang"></a>

create database connection

	func getCredentials()(DBCredentials) {
		creds := NewDBCredentials()
		creds.SetHost("localhost")
		creds.SetPort(3306)
		creds.SetName("complexdb1")
		creds.SetUser("myuser")
		creds.SetPass("mypass")
		return creds
	}
	
	func getConfig()(DBConfig) {	
		conf,err := NewDBConfig(MYSQL)
		if err != nil {
			return nil
		}
		return conf
	}
	
	func getConnection()(DBHandle) {
		dbh, err := NewDBHandle(getConfig())
		if err != nil {
			return nil
		}
		if err = dbh.Connect(getCredentials()); err != nil {
			return nil
		}
		return dbh
	}

insert rows into exampledb.exampletable:

	tbl := New_exampledb_exampletable()
	tbl.Set_teststr("test")
	tbl.Set_testfloat(11.12)
	tbl.Set_testint(10)
	tbl.Set_testdate(time.Now())
	if err := Insert_exampledb_exampletable(dbh,tbl); err != nil {
		return nil
	}

delete rows from exampledb.exampletable:

	stmt := NewDeleteStmt_exampledb_exampletable()
	
	vals := []uint16 { 10 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_exampledb_exampletable_ID(),WhereNotEqual(),ifval)
	
	if err := dbh.Delete(stmt); err != nil {
		return nil
	}

select rows from exampledb.exampletable:

	stmt := NewSelectStmt_exampledb_exampletable()
	
	vals := []uint16 { 10 }
	ifval := make([]interface{}, len(vals))
	for i,v := range vals {
		ifval[i] = v 
	}
	
	stmt.Where().AppendCondition(Def_exampledb_exampletable_ID(),WhereEqual(),ifval)
	
	res,err := dbh.Select(stmt)
	if err != nil {
		return nil
	}
	
loop through result from select statement using 'res' from above:

	for {
		row,err := res.Next(dbh)
		if err != nil {
			return
		} else if row == nil {
			return
		}
		
		rowstr := ""
		for idx := range row.Columns() {
			v := row.GetByIndex(uint64(idx))
			if v == nil {
				return
			}
			tval := fmt.Sprintf("%v",v);
			rowstr += " | " + tval
		}
	}

## FAQ <a name="FAQ"></a>

### I found a bug, missing a feature or have a patch <a name="bugs"></a>
Please file either a bugreport, feature- or pull-request on the offical [github](https://github.com/M0WA/GoDB) page.

### GoDB cannot make use of the brand-specific featureset xyz, why should I use it then? <a name="brandfeatures"></a>
The advantages of the GoDB framework are that you gain complete database brand independency for your code as well as an increased security.
There is use cases where the performance of a brand-specific feature is more relevant than brand independency. In such a case, GoDB is not what
you are looking for.