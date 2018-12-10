# this file contains all targets concerning the c-library of the GoDB framework

clib:
	$(MAKE) $(LIBS_DIR)/c/configure
	( cd $(LIBS_DIR)/c && ./configure && $(MAKE) )

$(LIBS_DIR)/c/configure:
	( cd $(LIBS_DIR)/c && autoreconf -i && aclocal && automake )

copy_clib:
	if [ ! -d $(LIBS_DIR)/c ]; then mkdir -p $(LIBS_DIR)/c; fi
	rsync -crq static/c/* $(LIBS_DIR)/c/
	rsync -crq $(GENERATED_DIR)/c/* $(LIBS_DIR)/c/

clean_clib:
	if [ -d $(LIBS_DIR)/c ]; then $(RM) -rf $(LIBS_DIR)/c; fi

testbin_clib:
	$(MAKE) $(LIBS_DIR)/c/configure
	( cd $(LIBS_DIR)/c && ./configure --enable-testbin && $(MAKE) )
	
testbin_dbg_clib:
	$(MAKE) $(LIBS_DIR)/c/configure
	( cd $(LIBS_DIR)/c && ./configure --enable-testbin --enable-debug && $(MAKE) )

test_clib:
	$(MAKE) testbin_clib
	$(LIBS_DIR)/c/src/./libdbtest

valgrind_clib:
	$(MAKE) testbin_dbg_clib
	( cd  $(LIBS_DIR)/c/src/ && valgrind --trace-children=yes --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all -v ./libdbtest )

.PHONY: clib copy_clib clean_clib testbin_clib test_clib valgrind_clib