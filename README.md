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
    1. [I found a bug, missing a feature or have a patch](#Bugs)
    2. [Missing brand-specific features](#BrandFeatures)


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
<tr><td>Index</td><td>yes</td></tr>
<tr><td>Join/ForeignKey</td><td>yes</td></tr>
<tr><td>UniqueKey</td><td>yes</td></tr>
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

### Code Generator <a name="CodeGenerator"></a>

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
in [API](#API).

## API <a name="API"></a>

This section gives an overview of the available configuration settings for the [generator](#YamlFormat)
or language specific APIs.

### YAML Format <a name="YamlFormat"></a>

The following is a full-featured example database structure of a database named exampledb with two tables exampletable1 and exampletable2.
Most of the API documentation example refer to this structure.

	---
	databases:
	  - name: exampledb1
	    tables:
	      - name: exampletable1
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
	      - name: exampletable2
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
	        indexkeys:
	          - column: testfloat
	          - column: teststr


### C <a name="CLib"></a>

The C library is the core implementation for all/most other languages.
Except for the init_dblib() function, GoDB is thread-safe. Multiple
database connections can be created and shared in between threads as
long as the access to a single database connection (struct DBHandle)
is syncronized by you. Best practise would be to use one database connection
per thread.

#### Connect to a database

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
		return 1; }
	
	// set credentials
	DBCredentials creds = () {
		.host = "localhost",
		.port = 3306,
		.name = "exampledb1",
		.user = "myuser",
		.pass = "mypass",
	};
	
	// connect to database
	if( connect_db(dbh,&creds) ) {
		return 1; }

#### Disconnect from a database
	
	// disconnect from database
	if ( disconnect_db(dbh) ) {
		return 1; }
	
	// destroy the database handle
	if( destroy_dbhandle(dbh) ) {
		return 1; }
	
	// shutdown library
	exit_dblib();

#### Creating a table structure and set values on it

	//
	// initialize structure named "tbl"
	// representing table "table1" in database "database1"
	//
	INIT_TABLE(exampledb1,exampletable1,tbl);
	
	// set the values of our table		
	sprintf(exampledb1_exampletable1_set_teststr(tbl,row),"test"); 
	*(exampledb1_exampletable1_set_testint(tbl,row)) = 10;
	*(exampledb1_exampletable1_set_testfloat(tbl,row)) = 10.10;	
	
	// set a datetime type variable
	time_t testdate = time(0);
	memset(exampledb1_exampletable1_set_testdate(tbl,row),0,sizeof(struct tm));
	gmtime_r(&testdate,exampledb1_exampletable1_set_testdate(tbl,row)); 
	
	// destroy the "tbl" when done
	destroy_dbtable(&tbl.dbtbl);
	
	//
	// initialize structure tbl2 that can contain multiple rows
	// (used in bulk inserts)
	//
	size_t initRows = 2;
	INIT_TABLE_ROWS(exampledb1,exampletable1,tbl2,initRows);
	
	// set the values in different rows 
	*(exampledb1_exampletable1_set_testint(tbl2,0)) = 10; 
	*(exampledb1_exampletable1_set_testint(tbl2,1)) = 10;
		
	// destroy the "tbl2" when done
	destroy_dbtable(&tbl2.dbtbl);

#### Insert a row into a table

	// create and initialize a structure named "tbl" or 
	// "tbl2" for bulk inserts as seen above
	// then prepare the insert statement
	InsertStmt s;
	if( create_insert_stmt(&s,&tbl.dbtbl) ) {
		return 1;
	}
	
	// execute the insert statement on a database connection "dbh"
	if( insert_db(dbh,&s)) {
		//error
		return 1;
	}	

#### Creating a Where Clause

A where clause is represented by the WhereClause datatype and consist of one or more
WhereComposite and WhereCondition datatypes.

The following statement datatypes support where clauses:
* SelectStmt
* DeleteStmt
* UpdateStmt

To append a WhereCondition to a WhereClause use the where_append() function:

	//get column definition, i.e. column testint 
	const DBColumnDef *coldef = exampledb1_exampletable1_coldef_testint();
	uint32 my_value = 32, my_value2 = 23;
	size_t value_count = 2;
		
	// pointers to values must be valid until
	// statement is used
	const void (*values)[] = { &my_value, &my_value2	};
	
	WhereCondition cond;
	memset(&cond,0,sizeof(WhereCondition));
	cond.cond = WHERE_EQUAL;
	cond.type = WHERE_COND;
	cond.def = coldef;
	cond.values = values;
	cond.cnt = value_count;
	
	SelectStmt s;
	...
	
	if( where_append(&s.where,(union _WhereStmt *)&cond) ) {
		return -1;
	}
	
Resulting where clause would be:

	WHERE colname = 32 OR colname = 23

WhereComposite datatypes combine multiple WhereCondition/WhereComposite using AND/OR,
therefore act like parathesis for more complex where clauses.

This is how WhereComposite/WhereCondition and WhereClause datatypes relate:

	WhereCondition1 := ( colname = 32 OR colname = 23 )
	WhereCondition2 := ( othercol = 'string' )
	WhereCondition3 := ( somecol = 10.10 )
	WhereComposite1 := ( WhereCondition... OR WhereCondition... )
	WhereComposite2 := ( WhereCondition1 AND WhereCondition2 OR WhereComposite1 )
	WhereClause     := ( WhereComposite2 AND WhereCondition3 )


#### Select rows from table

	// get table definition for table to select from
	const DBTableDef *tbldef = exampledb_exampletbl_tbldef();
	
	// initialize select statement from table definition
	SelectStmt stmt;
	if( create_select_stmt(&stmt,tbldef) ) {
		return 1;
	}
	
	// optionally create one or more where conditions (see Creating a Where Clause)
	WhereCondition cond;
	...
	if( where_append(&stmt.where,(union _WhereStmt *)&cond) ) {
		return 1; 
	}
	
	// initialize select result
	SelectResult res;
	memset(&res,0,sizeof(SelectResult));
	
	// actually select some rows
	if( select_db(dbh,&stmt,&res) ) {
		return 1; 
	}
	
	// do something with the result
	// see: Fetching a result set row by row 
		
	// do not forget to destroy stmt + result
	// once you are done	
	destroy_select_stmt(&stmt);
	destroy_selectresult(&res);

#### Fetching a result set row by row

	SelectResult res; //obtained by a select stmt (see: Select rows from table)
	
	// fetching results from database
	while( (rc = fetch_db(dbh,&res)) > 0){		
		const unsigned long long *tID = 0;
		const long *tTestInt = 0;
		const char *tTestStr = 0;
		const double *tTestDouble = 0;
		
		//each exampledb_exampletbl_result_XXX call takes the result
		//and sets the second argument to the value of the column XXX
		
		if( exampledb_exampletbl_result_ID(res, &tID) ) {
			return 1;
		} else if( exampledb_exampletbl_result_testint(res, &tTestInt) ) {
			return 1;
		} else if( exampledb_exampletbl_result_teststr(res, &tTestStr) ) {
			return 1;
		} else if( exampledb_exampletbl_result_testfloat(res, &tTestDouble) ) {
			return 1;
		}
		
		// do something with the values
	}
	
	// do not forget to destroy the select result
	destroy_selectresult(&res);

#### Deleting rows from table

	DeleteStmt stmt;
	if( create_delete_stmt(&stmt,exampledb1_exampletable1_tbldef()) ) {
		return 1;
	}
	
	//prepare where condition as needed (see: Creating a Where Clause)
	struct _WhereCondition cond;
	if( where_append(&stmt.where,(union _WhereStmt *)&cond) ) {
		return 1;
	}
	
	// execute delete statement
	if(delete_db(dbh,&stmt))  {
		return 1;
	}
	
	// do not forget to destroy the delete statement
	destroy_delete_stmt(&stmt);

### Golang <a name="Golang"></a>

The golang API is a thin layer on top of the c-api explained above.
It wraps the c library and also adds generated datatypes and functions
according to the database structure.

#### Connect to a database

The getConnection() function returns a connected DBHandle interface on success which can be used for later operations.
It's the correspondance of a database connection.

	func getCredentials()(DBCredentials) {
		creds := NewDBCredentials()
		creds.SetHost("localhost")
		creds.SetPort(3306)
		creds.SetName("db1")
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

#### Disconnect from a database

	var dbh DBHandle = ... //see: Connect to a database
	if err := dbh.Disconnect(); err != nil {
		return err
	}

#### Insert row into a table

	tbl := NewExampledb_Exampletbl(1)
	s := tbl.InsertStmt()
		
	tbl.SetTestint(33,0)
	tbl.SetTeststr("test",0)
	tbl.SetTestfloat(10.10,0)
	tbl.SetTestdate(time.Now(),0)
		
	if dbh.Insert(s) != nil {
		t.Fatal("error while insert");
	}

## FAQ <a name="FAQ"></a>

### I found a bug, missing a feature or have a patch <a name="Bugs"></a>
Please file either a bugreport, feature- or pull-request on the offical [github](https://github.com/M0WA/GoDB) page.

### GoDB cannot make use of the brand-specific featureset xyz, why should I use it then? <a name="BrandFeatures"></a>
The advantages of the GoDB framework are that you gain complete database brand independency for your code as well as an increased security.
There is use cases where the performance of a brand-specific feature is more relevant than brand independency. In such a case, GoDB is not what you are looking for.