# this file contains all targets concerning the golang-library of the GoDB framework

golib:
	( cd $(LIBS_DIR)/golang && LD_LIBRARY_PATH=$(LIBS_DIR)/c/src/.libs LIBS_DIR=$(PWD)/$(LIBS_DIR) $(MAKE) lib )

copy_golib:
	if [ ! -d $(LIBS_DIR)/golang ]; then mkdir -p $(LIBS_DIR)/golang; fi
	rsync -crq static/golang/* $(LIBS_DIR)/golang/
	rsync -crq $(GENERATED_DIR)/golang/* $(LIBS_DIR)/golang/

clean_golib:
	if [ -d $(LIBS_DIR)/golang ]; then $(RM) -r $(LIBS_DIR)/golang; fi

test_golib:
	( cd $(LIBS_DIR)/golang && LIBS_DIR=$(PWD)/$(LIBS_DIR) $(MAKE) test )

.PHONY: golib copy_golib clean_golib test_golang