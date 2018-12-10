golib:
#	$(MAKE) copy_golib

copy_golib:
	if [ ! -d $(LIBS_DIR)/golang ]; then mkdir -p $(LIBS_DIR)/golang; fi
	cp -r static/golang/* $(LIBS_DIR)/golang/
	cp -r $(GENERATED_DIR)/golang/* $(LIBS_DIR)/golang/

clean_golib:
	if [ -d $(LIBS_DIR)/golang ]; then $(RM) -rf $(LIBS_DIR)/golang; fi

test_golib:
	( cd $(LIBS_DIR)/golang && LIBS_DIR=$(PWD)/$(LIBS_DIR) VERBOSE=1 make test )

.PHONY: golib copy_golib clean_golib test_golang