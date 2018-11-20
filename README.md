# GoDB - database framework

GoDB's main goal is to abstract code from your database manufacturer while giving a strong binding to the actual database structure.
It is written with a strong consideration of limited resources like memory and cpu power to be "big-data ready".

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

## Usage <a name="Usage"></a>
get and build the library:<br><br>
<code>
git clone https://github.com/M0WA/GoDB<br>
cd GoDB<br>
cp db.yaml.example db.yaml<br>
DATABASE_YAML=db.yaml make
</code>
<br><br>
initialize the database (i.e. MySQL):<br><br>
<code>
cd generated/sql/mysql/<br>
cat * | mysql<br>
</code>

### API <a name="API"></a>

## Components <a name="Components"></a>

### Code Generator <a name="Code Generator"></a>

### Language Libraries <a name="Library"></a>