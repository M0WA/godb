.PHONY: generate generator

generator:
	( cd src && BIN_DIR=$(PWD)/bin $(MAKE) )

generate:
	BIN_DIR=$(PWD)/bin $(MAKE) generator
	bin/./generator -f $(DATABASE_YAML) -o $(GENERATED_DIR) -t $(TMPL_DIR)