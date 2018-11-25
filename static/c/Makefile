LIBNAME=libDB
OBJPATH=obj
INCLUDES=-I ./include

GDB=

TESTCFLAGS=
ifdef TEST
TESTCFLAGS:=$(CFLAGS) -D_DB_TEST_VERSION
endif

MYSQLCFLAGS=
MYSQLLDFLAGS=-lmysqlclient
ifdef DISABLE_MYSQL
MYSQLCFLAGS=-D_DISABLE_MYSQL
MYSQLLDFLAGS=
endif

POSTGRESCFLAGS=
POSTGRESLDFLAGS=-lpq
ifdef DISABLE_POSTGRES
POSTGRESCFLAGS=-D_DISABLE_POSTGRES
POSTGRESLDFLAGS=
endif

DBICFLAGS=
DBILDFLAGS=-ldbi
ifdef DISABLE_DBI
DBICFLAGS=-D_DISABLE_DBI
DBILDFLAGS=
endif

ifdef DISABLE_POSTGRES
ifdef DISABLE_MYSQL
ifdef DISABLE_DBI
$(error no database type selected)
endif
endif
endif

HDRS = $(wildcard */*.h)
SRCS = $(wildcard */*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

ifdef DEBUG
GDB=gdb
BUILDTYPEFLAGS=-ggdb -O0 -D_GODB_DEBUG
LIBNAME:=$(LIBNAME)Debug
else
BUILDTYPEFLAGS=-g -O2 -Werror
endif

CFLAGS =-Wall -fPIC -D_REENTRANT -pthread -D_XOPEN_SOURCE -D_GNU_SOURCE $(BUILDTYPEFLAGS) $(INCLUDES) $(TESTCFLAGS) $(MYSQLCFLAGS) $(POSTGRESCFLAGS) $(DBICFLAGS)
LDFLAGS=$(MYSQLLDFLAGS) $(POSTGRESLDFLAGS) $(DBILDFLAGS)

.PHONY: all clean test testbin

default_target: all
all: $(LIBNAME).so

clean:
	@find . -name "*.o"  -delete
	@find . -name "*.d"  -delete
	@find . -name "*.a"  -delete
	@find . -name "*.so" -delete

test:
	$(MAKE) testbin
	$(MAKE) TEST=1 testrun

testbin:
	$(MAKE) clean
	$(MAKE) TEST=1 all
	$(CC) $(CFLAGS) -D_DB_TEST_VERSION -o test $(OBJS) $(LDFLAGS)

testrun:
	$(GDB) ./test

valgrind:
	DEBUG=1 $(MAKE) testbin
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all -v ./test

$(LIBNAME).so: $(patsubst %.c, %.o, $(OBJS))
	$(CC) $(CFLAGS) -shared -o $(LIBNAME).so $(OBJS) $(LDFLAGS)

$(LIBNAME).a: $(patsubst %.c, %.o, $(OBJS))
	ar rs $(MAIN).a $^

-include $(DEPS)

.c.o:
	$(CC) $(CFLAGS) -MP -MMD -c $^ -o $@

