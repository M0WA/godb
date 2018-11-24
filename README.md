# GoDB - database framework

GoDB's main goal is to abstract code from your database manufacturer while giving a strong binding to the actual database structure.
It is written with a strong consideration of limited resources like memory and cpu power to be "big-data ready".
Think of it as an advanced version of the well known libdbi.

For more information visit the GoDB [homepage](https://go-db.net) and the public [github](https://github.com/M0WA/GoDB) page.

## Table of contents
1. [Features](#Features)
2. [Usage/Install](#Usage)
3. [API](#API)
4. [Components](#Components)
      1. [Code Generator](#Code Generator)
      2. [Library](#Library)



## Features <a name="Features"></a>
Supported databases:<br>

<table border=1>
<tr><td>Manufacturer</td><td>DBI</td><td>NATIVE</td></tr>
<tr><td>MYSQL</td><td>TBD</td><td>TBD</td></tr>
<tr><td>PostgreSQL</td><td>TBD</td><td>TBD</td></tr>
<tr><td>Oracle DB</td><td>TBD</td><td>TBD</td></tr>
<tr><td>IBM DB2</td><td>TBD</td><td>TBD</td></tr>
</table>

<br><br>
Supported languages:<br><br>

<table border=1>
<tr><td>Languages</td><td>State</td></tr>
<tr><td>C</td><td>TBD</td></tr>
<tr><td>C++</td><td>TBD</td></tr>
<tr><td>Golang</td><td>TBD</td></tr>
<tr><td>Python</td><td>TBD</td></tr>
</table>

* Prepared Statements (where applicable)
* Maximum Brand Independance
* Full Language Flexibility
* Extensive Test Framework
* Minimal Memory + CPU Footprint
* Open Source
* Type Safety

## Usage <a name="Usage"></a>
get and build the example library:

	git clone https://github.com/M0WA/GoDB
	cd GoDB
	cp db.yaml.example db.yaml
	DATABASE_YAML=db.yaml make

create the example database:

	# for mysql databases
	cat generated/sql/mysql/* | sudo mysql -u root

    # for postgresql databases
    cat generated/sql/postgre/* | sudo -u postgres psql -U postgres

initialize the example database for tests:

	# mysql init statement:
	echo "insert into complexdb1.complextable1 (ID,testint,teststr,testfloat,testdate) VALUES(10,10,'test',10.10,NOW());" | sudo mysql
	
	# postgresql init statement:
	echo "insert into complextable1 (ID,testint,teststr,testfloat,testdate) VALUES(10,10,'test',10.10,NOW());" | sudo -u postgres psql -U postgres -d complexdb1

test library:

	make test

## API <a name="API"></a>

### C

connect to mysql database:

	// initialize library
	init_dblib();
	
	// settings
	DBConfig config = (DBConfig){
		.type = DB_TYPE_MYSQL,
		.mysql.autoreconnect = 1,
		.mysql.compression = 1,
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

	TABLE_STRUCT(exampledb,exampletable,tbl);
	
	SelectStmt stmt;
	memset(&stmt,0,sizeof(SelectStmt));
	stmt.defs = tbl.dbtbl.def->cols;
	stmt.ncols = tbl.dbtbl.def->ncols;
	
	char test[] = "test";
	
	struct _WhereCondition cond;
	memset(&cond,0,sizeof(struct _WhereCondition));
	cond.cond = WHERE_EQUAL;
	cond.type = WHERE_COND;
	cond.def = &(tbl.dbtbl.def->cols[1]);
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

delete rows from exampledb.exampletable including where clause:

	TABLE_STRUCT(exampledb,exampletable,tbl);
	
	int id = 10;
	
	struct _WhereCondition cond;
	memset(&cond,0,sizeof(struct _WhereCondition));
	cond.cond = WHERE_NOT_EQUAL;
	cond.type = WHERE_COND;
	cond.def = &(tbl.dbtbl.def->cols[0]);
	cond.values = (const void*[]){&id};
	cond.cnt = 1;
	
	DeleteStmt stmt;
	memset(&stmt,0,sizeof(DeleteStmt));
	stmt.def = tbl.dbtbl.def;
	
	if( where_append(&stmt.where,(union _WhereStmt *)&cond) ) {
		LOG_FATAL(1,"exampledb.exampletable: cannot append where stmt"); }
	
	if(delete_db(dbh,&stmt)) {
		LOG_FATAL(1,"exampledb.exampletable: error while delete"); }
	
	DESTROY_STMT(&stmt);
	
## Components <a name="Components"></a>

### Code Generator <a name="Code Generator"></a>

### Language Libraries <a name="Library"></a>