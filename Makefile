DATABASE_YAML=db.yaml

TMPL_DIR=tmpl
GENERATED_DIR=generated
LIBS_DIR=libs

default_target: all

all:
	$(MAKE) generate
	$(MAKE) lib

clean:
	$(MAKE) cleanlibs
	$(MAKE) cleangenerator

include mk/generator.mk
include mk/libs.mk
include mk/tests.mk

.PHONY: all clean
