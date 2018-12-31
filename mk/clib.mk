# this file contains all targets concerning the c-library of the GoDB framework

ifeq ($(DISABLE_POSTGRES),1)
ifeq ($(DISABLE_MYSQL),1)
ifeq ($(DISABLE_DBI),1)
$(error no database type selected)
endif
endif
endif

ifeq ($(DISABLE_POSTGRES),1)
	CLIB_CONFIGURE_PARAM += --enable-postgre=no
endif
ifeq ($(DISABLE_MYSQL),1)
	CLIB_CONFIGURE_PARAM += --enable-mysql=no
endif
ifeq ($(DISABLE_DBI),1)
	CLIB_CONFIGURE_PARAM += --enable-dbi=no
endif
ifeq ($(DEBUG),1)
	CLIB_CONFIGURE_PARAM += --enable-debug
endif

clib:
	$(MAKE) $(LIBS_DIR)/c/configure
	( cd $(LIBS_DIR)/c && ./configure $(CLIB_CONFIGURE_PARAM) && $(MAKE) )

$(LIBS_DIR)/c/configure:
	( cd $(LIBS_DIR)/c && autoreconf -i && aclocal && automake )

copy_clib:
	if [ ! -d $(LIBS_DIR)/c ]; then mkdir -p $(LIBS_DIR)/c; fi
	rsync -crq static/c/* $(LIBS_DIR)/c/
	rsync -crq $(GENERATED_DIR)/c/* $(LIBS_DIR)/c/

clean_clib:
	if [ -d $(LIBS_DIR)/c ]; then $(RM) -r $(LIBS_DIR)/c; fi

testbin_clib:
	$(MAKE) $(LIBS_DIR)/c/configure
	( cd $(LIBS_DIR)/c && ./configure $(CLIB_CONFIGURE_PARAM) --enable-testbin && $(MAKE) )
	
testbin_dbg_clib:
	$(MAKE) $(LIBS_DIR)/c/configure
	( cd $(LIBS_DIR)/c && ./configure $(CLIB_CONFIGURE_PARAM) --enable-testbin --enable-debug && $(MAKE) )

test_clib:
	$(MAKE) testbin_clib
	LD_LIBRARY_PATH=$(LIBS_DIR)/c/src/.libs $(LIBS_DIR)/c/src/.libs/./libdbtest

valgrind_clib:
	$(MAKE) testbin_dbg_clib
	LD_LIBRARY_PATH=$(LIBS_DIR)/c/src/.libs valgrind --trace-children=yes --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all -v $(LIBS_DIR)/c/src/.libs/./libdbtest

gdb_clib:
	$(MAKE) testbin_dbg_clib
	LD_LIBRARY_PATH=$(LIBS_DIR)/c/src/.libs gdb $(LIBS_DIR)/c/src/.libs/libdbtest

.PHONY: clib copy_clib clean_clib testbin_clib test_clib valgrind_clib gdb_clib