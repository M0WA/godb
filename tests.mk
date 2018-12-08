.PHONY: preparetest test valgrind gprof mtrace test_generator test_clib test_golang

preparetest:
	$(MAKE) clean
	$(MAKE) cleanlibs
	$(MAKE) generator
	bin/./generator -l | bin/./generator -o $(GENERATED_DIR) -t $(TMPL_DIR)
	$(MAKE) copylib
	
test:
	$(MAKE) test_generator
	$(MAKE) test_clib
	$(MAKE) test_golang
	$(MAKE) clean

test_generator:
	( cd src && $(MAKE) test )
	
test_golang:
	$(MAKE) preparetest
	( cd $(LIBS_DIR)/c && $(MAKE) )
	( cd $(LIBS_DIR)/golang && LIBS_DIR=$(PWD)/$(LIBS_DIR) VERBOSE=1 make test )

test_clib:
	$(MAKE) preparetest
	( cd $(LIBS_DIR)/c && $(MAKE) test )


# debugging/verbose testing for c lib
valgrind:
	$(MAKE) preparetest
	( cd $(LIBS_DIR)/c && $(MAKE) valgrind )
	$(MAKE) clean
	
gprof:
	$(MAKE) preparetest
	( cd $(LIBS_DIR)/c && $(MAKE) gprof )
	$(MAKE) clean
	
mtrace:
	$(MAKE) preparetest
	( cd $(LIBS_DIR)/c && $(MAKE) mtrace )
	$(MAKE) clean