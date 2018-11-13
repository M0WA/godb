DATABASE_YAML=db.yaml
TMPL_DIR=tmpl
GENERATED_DIR=generated
LIBS_DIR=libs

.PHONY: all generate lib clean

all:
	$(MAKE) generate
	$(MAKE) lib

generate:
	-mkdir bin
	go build -o bin/generator generator
	bin/./generator -f $(DATABASE_YAML) -o $(GENERATED_DIR) -t $(TMPL_DIR)

lib:
	-mkdir -p $(LIBS_DIR)
	cp -r static/* $(LIBS_DIR)
	cp -r $(GENERATED_DIR)/* $(LIBS_DIR)
	( cd $(LIBS_DIR)/c && $(MAKE) )

clean:
	rm -rf $(GENERATED_DIR) $(LIBS_DIR) >/dev/null 2>&1