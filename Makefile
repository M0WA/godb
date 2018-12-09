DATABASE_YAML=db.yaml

TMPL_DIR=tmpl
GENERATED_DIR=generated
LIBS_DIR=libs

default_target: all

all:
	$(MAKE) generate
	$(MAKE) libs

clean:
	$(MAKE) clean_libs
	$(MAKE) clean_generator

include mk/generator.mk
include mk/clib.mk
include mk/golib.mk
include mk/libs.mk
include mk/tests.mk

.PHONY: all clean
