.PHONY: cleanlibs lib copylib

copylib:
	-mkdir -p $(LIBS_DIR)
	cp -r static/* $(LIBS_DIR)
	cp -r $(GENERATED_DIR)/* $(LIBS_DIR)

lib:
	$(MAKE) copylib
	( cd $(LIBS_DIR)/c && $(MAKE) )
	
cleanlibs:
	rm -rf $(LIBS_DIR) >/dev/null 2>&1