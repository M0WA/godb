copylib:
	if [ ! -d $(LIBS_DIR) ]; then mkdir -p $(LIBS_DIR); fi
	cp -r static/* $(LIBS_DIR)
	cp -r $(GENERATED_DIR)/* $(LIBS_DIR)

lib:
	$(MAKE) copylib
	( cd $(LIBS_DIR)/c && autoreconf -i && aclocal && automake && ./configure && make )
#	( cd $(LIBS_DIR)/c && $(MAKE) )
	
cleanlibs:
	rm -rf $(LIBS_DIR) >/dev/null 2>&1

.PHONY: cleanlibs lib copylib