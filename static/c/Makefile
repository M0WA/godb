ifeq ($(DISABLE_POSTGRES),1)
ifeq ($(DISABLE_MYSQL),1)
ifeq ($(DISABLE_DBI),1)
$(error no database type selected)
endif
endif
endif

LIBNAME=libDB
OBJPATH=obj
INCLUDES=-I ./include

GDB=

TESTCFLAGS=
ifeq ($(TEST),1)
TESTCFLAGS=-D_DB_TEST_VERSION
endif

ifeq ($(DISABLE_MYSQL),1)
MYSQLCFLAGS=-D_DISABLE_MYSQL
MYSQLLDFLAGS=
ENABLE_MYSQLHELPER = 0
else
MYSQLCFLAGS=
MYSQLLDFLAGS=-lmysqlclient
ENABLE_MYSQLHELPER = 1
endif

ifeq ($(DISABLE_POSTGRES),1)
POSTGRESCFLAGS=-D_DISABLE_POSTGRES
POSTGRESLDFLAGS=
ENABLE_POSTGRESHELPER = 0
else
POSTGRESCFLAGS=
POSTGRESLDFLAGS=-lpq
ENABLE_POSTGRESHELPER = 1
endif

ifeq ($(DISABLE_DBI),1)
DBICFLAGS=-D_DISABLE_DBI
DBILDFLAGS=
else
DBICFLAGS=
DBILDFLAGS=-ldbi
ENABLE_POSTGRESHELPER = 1
ENABLE_MYSQLHELPER = 1
endif

POSTGRESHELPERCFLAGS=
ifeq ($(ENABLE_POSTGRESHELPER),1)
POSTGRESHELPERCFLAGS=-D_ENABLE_POSTGRESHELPER
endif

MYSQLHELPERCFLAGS=
ifeq ($(ENABLE_MYSQLHELPER),1)
MYSQLHELPERCFLAGS=-D_ENABLE_MYSQLHELPER
endif

HDRS = $(wildcard */*.h)
SRCS = $(wildcard */*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

ifeq ($(DEBUG),1)
GDB=gdb
BUILDTYPEFLAGS=-ggdb -O0 -D_GODB_DEBUG
LIBNAME:=$(LIBNAME)Debug
else
BUILDTYPEFLAGS=-g -O2 -Werror
GDB=
endif

GPROFCFLAGS=
ifeq ($(GPROF),1)
GPROFCFLAGS=-pg
endif

LDFLAGS=$(MYSQLLDFLAGS) $(POSTGRESLDFLAGS) $(DBILDFLAGS)
CFLAGS =-Wall -fPIC -D_REENTRANT -pthread -D_XOPEN_SOURCE -D_GNU_SOURCE \
$(BUILDTYPEFLAGS) $(GPROFCFLAGS) $(INCLUDES) $(TESTCFLAGS) \
$(MYSQLCFLAGS) $(POSTGRESCFLAGS) $(DBICFLAGS) \
$(MYSQLHELPERCFLAGS) $(POSTGRESHELPERCFLAGS)

.PHONY: all clean test testbin testrun valgrind gprof

default_target: all
all: $(LIBNAME).so

clean:
	@find . -name "*.o"  -delete
	@find . -name "*.d"  -delete
	@find . -name "*.a"  -delete
	@find . -name "*.so" -delete
	-rm gmon.out

test:
	TEST=1 $(MAKE) testbin
	$(MAKE) testrun

testbin:
	$(MAKE) clean
	TEST=1 $(MAKE) all
	$(CC) $(CFLAGS) -D_DB_TEST_VERSION -o test $(OBJS) $(LDFLAGS)

testrun:
	$(GDB) ./test

valgrind:
	DEBUG=1 $(MAKE) testbin
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all -v ./test

gprof:
	GPROF=1 $(MAKE) testbin
	./test
	gprof test | less
	rm gmon.out

$(LIBNAME).so: $(patsubst %.c, %.o, $(OBJS))
	$(CC) $(CFLAGS) -shared -o $(LIBNAME).so $(OBJS) $(LDFLAGS)

$(LIBNAME).a: $(patsubst %.c, %.o, $(OBJS))
	ar rs $(MAIN).a $^

-include $(DEPS)

.c.o:
	$(CC) $(CFLAGS) -MP -MMD -c $^ -o $@

