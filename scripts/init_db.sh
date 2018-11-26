#!/bin/bash

set -e

DBTYPES=(mysql postgres)

DBHOST=localhost
DBTYPE=

DBUSER=
DBPASS=

SQLDIR=generated/sql

function usage() {
	echo "$0 -U <user> [ -P <pass> -H <host> -T <dbtype> -D <sqldir> ]"
	echo "	-U <user>: user to create"
	echo "	-P <pass>: password for created user, read from stdin if unspecified"
	echo "	-H <host>: hostname/ip of database server (default: $DBHOST)"
	echo "	-T <type>: type(-s) of database to initialize (default: '${DBTYPES[@]}')"
	echo "	-D <sql> : directory with sql files (default: '$SQLDIR')"
	echo "	-v       : verbose"
	echo "	-h       : help"
}

function check_param() {
	if [ -z $DBUSER ]; then
		echo "invalid user (-U): $DBUSER"
		exit 1
	fi
	if [[ -z $DBHOST ]]; then
		echo "invalid host (-H)"
		exit 1
	fi
	if [[ -z $DBPASS ]]; then
		echo -n "new user's password: "
		read -s DBPASS
		echo
	fi
}

function init_mysql() {
	echo -n "connecting to mysql as root, "
SQLSTMT="
INSERT INTO complexdb1.complextable1 (ID,testint,teststr,testfloat,testdate) VALUES(10,10,'bla',10.10,NOW()); \
INSERT INTO complexdb1.complextable2 (ID,testint,teststr,testfloat,testdate,testfk) VALUES(10,10,'bla',10.10,NOW(),10); \
DROP USER IF EXISTS '$DBUSER'@'$DBHOST'; \
CREATE USER '$DBUSER'@'$DBHOST' IDENTIFIED BY '$DBPASS'; \
GRANT ALL ON *.* TO '$DBUSER'@'$DBHOST'; \
FLUSH PRIVILEGES; \
"
	( echo "DROP DATABASE IF EXISTS complexdb1; " && cat $SQLDIR/mysql/* && echo "$SQLSTMT" ) | mysql -uroot -p
}

function init_postgres() {
SQLSTMT="\\c complexdb1;
DROP USER IF EXISTS $DBUSER; \
CREATE USER $DBUSER WITH ENCRYPTED PASSWORD '$DBPASS'; \
GRANT ALL ON DATABASE complexdb1 TO $DBUSER; \
GRANT ALL ON TABLE complextable1 TO $DBUSER; \
GRANT ALL ON TABLE complextable2 TO $DBUSER; \
GRANT ALL ON SEQUENCE complextable1_id_seq TO $DBUSER; \
GRANT ALL ON SEQUENCE complextable2_id_seq TO $DBUSER; \
INSERT INTO complextable1 (ID,testint,teststr,testfloat,testdate) VALUES(10,10,'bla',10.10,NOW()); \
INSERT INTO complextable2 (ID,testint,teststr,testfloat,testdate,testfk) VALUES(10,10,'bla',10.10,NOW(),10); \
"
	( echo "DROP DATABASE IF EXISTS complexdb1; " && cat $SQLDIR/postgre/* && echo "$SQLSTMT" ) | sudo -u postgres psql -U postgres -d postgres
}

function init_db() {
	for elem in "${DBTYPES[@]}"
	do
		if [[ -z $DBTYPE || "${elem}" == "$DBTYPE" ]];  then
			init_$elem
		fi
	done
}

while getopts ":hvU:P:H:T:" opt; do
	case $opt in
	h)
		usage
		exit 0
		;;
	v)
		set -x
		;;
	U)
		DBUSER=$OPTARG
		;;
	P)
		DBPASS=$OPTARG
		;;
	H)
		HOST=$OPTARG
		;;
	T)
		DBTYPE=$OPTARG
		;;
	D)
		SQLDIR=$OPTARG
		;;
	\?)
		echo "invalid option: -$OPTARG" >&2
		exit 0
		;;
	esac
done

check_param
init_db
