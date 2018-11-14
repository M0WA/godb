DATABASE_YAML=db.yaml

TMPL_DIR=tmpl
GENERATED_DIR=generated
LIBS_DIR=libs

.PHONY: all generate generator cleangenerator testgenerator cleanlibs lib copylib clean test

default_target: all

all:
	$(MAKE) generate
	$(MAKE) lib
	
generator:
	-mkdir bin
	go build generator/layout
	go build generator/dml
	go build generator/ddl
	go build -o bin/generator generator
	
cleangenerator:
	rm -rf $(GENERATED_DIR) >/dev/null 2>&1
	go clean generator/layout
	go clean generator/dml
	go clean generator/ddl
	go clean generator
	
testgenerator:
	go test generator/layout
	go test generator/dml
	go test generator/ddl
	go test generator

generate:
	$(MAKE) generator
	bin/./generator -f $(DATABASE_YAML) -o $(GENERATED_DIR) -t $(TMPL_DIR)
	
copylib:
	-mkdir -p $(LIBS_DIR)
	cp -r static/* $(LIBS_DIR)
	cp -r $(GENERATED_DIR)/* $(LIBS_DIR)

lib:
	$(MAKE) copylib
	( cd $(LIBS_DIR)/c && $(MAKE) )
	
cleanlibs:
	rm -rf $(LIBS_DIR) >/dev/null 2>&1

clean:
	$(MAKE) cleangenerator
	$(MAKE) cleanlibs
	
test:
	$(MAKE) clean
	$(MAKE) generator
	$(MAKE) testgenerator
	$(MAKE) cleanlibs
	bin/./generator -l | bin/./generator -o $(GENERATED_DIR) -t $(TMPL_DIR)
	$(MAKE) copylib
	( cd $(LIBS_DIR)/c && $(MAKE) test )
	$(MAKE) clean
	