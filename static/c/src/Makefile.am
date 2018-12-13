lib_LTLIBRARIES = libDB.la

libDB_la_CFLAGS = -Wall -fPIC -D_REENTRANT -pthread -D_XOPEN_SOURCE -D_GNU_SOURCE
libDB_la_LDFLAGS =

if DEBUG
libDB_la_CFLAGS += -ggdb -O0 -D_GODB_DEBUG
else
libDB_la_CFLAGS += -g -O2 -Werror
endif

if ! DISABLE_POSTGRES
libDB_la_LDFLAGS += -lpq
endif

if ! DISABLE_MYSQL
libDB_la_LDFLAGS += -lmysqlclient
endif

if ! DISABLE_DBI
libDB_la_LDFLAGS += -ldbi
endif

if ENABLE_POSTGRESHELPER
libDB_la_CFLAGS += -D_ENABLE_POSTGRESHELPER
endif

if ENABLE_MYSQLHELPER
libDB_la_CFLAGS += -D_ENABLE_MYSQLHELPER
endif

# find libs/c/src -name *.c | xargs -l1 basename | sort
libDB_la_SOURCES = \
column.c \
db.c \
dbihelper.c \
dbihooks.c \
dbtypes.c \
helper.c \
join.c \
logger.c \
mysqlbind.c \
mysqlhelper.c \
mysqlhooks.c \
postgresbind.c \
postgreshelper.c \
postgreshooks.c \
selectresult.c \
statements.c \
stringbuf.c \
table.c \
tables.c \
values.c \
where.c

if TESTBIN
bin_PROGRAMS = libdbtest
libdbtest_SOURCES = test.c tests.c
libdbtest_CFLAGS = -D_DB_TEST_VERSION $(libDB_la_CFLAGS)
libdbtest_LDADD = libDB.la
endif
