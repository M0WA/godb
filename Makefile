DATABASE_YAML=db.yaml

TMPL_DIR=tmpl
GENERATED_DIR=generated
LIBS_DIR=libs


.PHONY: all clean

default_target: all

all:
	$(MAKE) generate
	$(MAKE) lib

clean:
	rm -rf $(GENERATED_DIR) >/dev/null 2>&1
	$(MAKE) cleanlibs

include generator.mk
include libs.mk
include tests.mk