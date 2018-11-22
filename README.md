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

* Prepared Statements
* Maximum Brand Independance
* Full Language Flexibility
* Extensive Test Framework
* Minimal Memory + CPU Footprint
* Open Source

## Usage <a name="Usage"></a>
get and build the library:

	git clone https://github.com/M0WA/GoDB
	cd GoDB
	cp db.yaml.example db.yaml
	DATABASE_YAML=db.yaml make

initialize the database (i.e. MySQL):

	cd generated/sql/mysql/
	cat * | mysql

## API <a name="API"></a>

### C

connect to mysql database:

	init_dblib();
	
	DBHandle *dbh = create_dbhandle(DBTYPE_MYSQL);
	if ( !dbh ) {
		LOG_FATAL(1,"create_dbhandle() failed"); }
	
	if( connect_db(dbh,"localhost",3306,"mydb","myuser","mypass") ) {
		LOG_FATAL(1,"connect_db() failed"); }
	
	// -> do some work here <-
		
	if ( disconnect_db(dbh) ) {
		LOG_FATAL(1,"disconnect_db() failed"); }
	
	if( destroy_dbhandle(dbh) ) {
		LOG_FATAL(1,"destroy_dbhandle() failed"); }
		
	exit_dblib();

insert rows into exampledb.exampletable:

	TABLE_STRUCT(exampledb,exampletable,tbl);
	set_example_values(&tbl);
	
	// single row insert
	if( INSERT_ONE_DBTABLE(dbh,&tbl.dbtbl) ) {
		LOG_FATAL(1,"insert failed"); }
	
	// bulk insert (multiple rows)
	TABLE_STRUCT(exampledb,exampletable,tbl2);
	set_example_values(&tbl2);
	
	DBTable* rows[2] = { &(tbl.dbtbl), &(tbl2.dbtbl) };
	const DBTable *const*const rowp = (const DBTable *const*const)&rows;
	if( insert_dbtable(dbh,rowp,2) ) {
		LOG_FATAL(1,"bulk insert failed"); }

## Components <a name="Components"></a>

### Code Generator <a name="Code Generator"></a>

### Language Libraries <a name="Library"></a>