ifeq ($(BIN_DIR),"")
BIN_DIR=../bin
endif

default_target: all

all:
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi
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
	if [ -f $(BIN_DIR) ]; then $(RM) $(BIN_DIR)/generator; fi

test:
	echo $(CGO_CFLAGS)
	go test generator/layout
	go test generator/dml
	go test generator/ddl
	go test generator

.PHONY: all clean test