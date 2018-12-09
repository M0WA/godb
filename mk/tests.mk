generate_testlib:
	bin/./generator -l | bin/./generator -o $(GENERATED_DIR) -t $(TMPL_DIR)
	$(MAKE) copy_libs
	
test:
	$(MAKE) clean
	$(MAKE) generator
	$(MAKE) test_generator
	$(MAKE) generate_testlib
	$(MAKE) test_clib
	$(MAKE) golib
	$(MAKE) test_golib
	$(MAKE) clean

# debugging/verbose testing for c lib
valgrind:
	$(MAKE) generate_testlib
	( cd $(LIBS_DIR)/c && $(MAKE) valgrind )
	$(MAKE) clean
	
gprof:
	$(MAKE) generate_testlib
	( cd $(LIBS_DIR)/c && $(MAKE) gprof )
	$(MAKE) clean
	
mtrace:
	$(MAKE) generate_testlib
	( cd $(LIBS_DIR)/c && $(MAKE) mtrace )
	$(MAKE) clean

.PHONY: generate_testlib test valgrind gprof mtrace