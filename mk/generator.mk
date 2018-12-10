# this file contains all targets concerning the code generator of the GoDB framework

generator:
	( cd src && BIN_DIR=$(PWD)/bin $(MAKE) )

generate:
	BIN_DIR=$(PWD)/bin $(MAKE) generator
	bin/./generator -f $(DATABASE_YAML) -o $(GENERATED_DIR) -t $(TMPL_DIR)

test_generator:
	( cd src && $(MAKE) test )

clean_generator:
	( cd src && BIN_DIR=$(PWD)/bin $(MAKE) clean )
	if [ -d $(GENERATED_DIR) ]; then rm -rf $(GENERATED_DIR); fi

.PHONY: generate generator test_generator clean_generator