cleangenerator:
	( cd src && BIN_DIR=$(PWD)/bin $(MAKE) clean )
	if [ -d $(GENERATED_DIR) ]; then rm -rf $(GENERATED_DIR); fi

generator:
	( cd src && BIN_DIR=$(PWD)/bin $(MAKE) )

generate:
	BIN_DIR=$(PWD)/bin $(MAKE) generator
	bin/./generator -f $(DATABASE_YAML) -o $(GENERATED_DIR) -t $(TMPL_DIR)

.PHONY: generate generator