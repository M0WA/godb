ifeq ($(BIN_DIR),"")
BIN_DIR=../bin
endif

.PHONY: all clean test

default_target: all

all:
	-mkdir $(BIN_DIR)
	go get gopkg.in/yaml.v1
	go build generator/layout
	go build generator/dml
	go build generator/ddl
	go build -o $(BIN_DIR)/generator generator

clean:
	go clean generator/layout
	go clean generator/dml
	go clean generator/ddl
	go clean generator
	rm $(BIN_DIR)/generator

test:
	go test generator/layout
	go test generator/dml
	go test generator/ddl
	go test generator