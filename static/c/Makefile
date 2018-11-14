LIBNAME=libDB
OBJPATH=obj
INCLUDES=-I ./include

GDB=

TESTCFLAGS=
ifdef TEST
TESTCFLAGS:=$(CFLAGS) -D_DB_TEST_VERSION
endif

HDRS = $(wildcard */*.h)
SRCS = $(wildcard */*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

ifdef DEBUG
GDB=gdb
BUILDTYPEFLAGS=-ggdb -O0
LIBNAME:=$(LIBNAME)Debug
else
BUILDTYPEFLAGS=-g -O2
endif

CFLAGS =-Wall -fPIC -D_REENTRANT -pthread -rdynamic $(BUILDTYPEFLAGS) $(INCLUDES) $(TESTCFLAGS)

.PHONY: all clean test

default_target: all
all: $(LIBNAME).so

clean:
	@find . -name "*.o"  -delete
	@find . -name "*.d"  -delete
	@find . -name "*.a"  -delete
	@find . -name "*.so" -delete
	
test:
	$(MAKE) TEST=1 testrun
	
testrun:
	$(MAKE) clean
	$(MAKE) TEST=1 all
	$(CC) $(CFLAGS) -D_DB_TEST_VERSION -o test $(OBJS) $(LDFLAGS)
	$(GDB) ./test
	

$(LIBNAME).so: $(patsubst %.c, %.o, $(OBJS))
	$(CC) $(CFLAGS) -shared -o $(LIBNAME).so $(OBJS) $(LDFLAGS)

$(LIBNAME).a: $(patsubst %.c, %.o, $(OBJS))
	ar rs $(MAIN).a $^

-include $(DEPS)

.c.o:
	$(CC) $(CFLAGS) -MP -MMD -c $^ -o $@
	
