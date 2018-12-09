copy_libs:
	$(MAKE) copy_clib
	$(MAKE) copy_golib

libs:
	$(MAKE) copy_libs
	$(MAKE) clib
	$(MAKE) golib
	
clean_libs:
	$(MAKE) clean_clib
	$(MAKE) clean_golib
	if [ -d $(LIBS_DIR) ]; then $(RM) -rf $(LIBS_DIR); fi

.PHONY: libs copy_libs clean_libs