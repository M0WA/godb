MALLOC_TRACE=mtrace.log

test:
	$(MAKE) testbin
	$(TARGETDIR)/test
	
testbin:
	TEST=1 $(MAKE) all
	$(CC) $(CFLAGS) -D_DB_TEST_VERSION -o $(TARGETDIR)/test $(OBJECTS) $(LIB)

valgrind:
	DEBUG=1 $(MAKE) testbin
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all -v $(TARGETDIR)/test

gprof:
	GPROF=1 $(MAKE) testbin
	$(TARGETDIR)/test
	gprof $(TARGETDIR)/test | less
	if [ -f gmon.out ];	then $(RM) gmon.out; fi

mtrace:
	$(MAKE) testbin
	MALLOC_TRACE=$(MALLOC_TRACE) $(TARGETDIR)/test
	if [ -f $(MALLOC_TRACE) ]; then \ 
		less $(MALLOC_TRACE) \
		rm $(MALLOC_TRACE) \
	fi

.PHONY: test testbin valgrind gprof mtrace