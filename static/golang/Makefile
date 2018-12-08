BUILDPARAM=

ifeq ($(VERBOSE),1)
BUILDPARAM=-x
endif

.PHONY: all lib test clean

default_target: all

all:
	$(MAKE) lib

clean:
	GOPATH=$(LIBS_DIR)/golang go clean godb

lib:
	GOPATH=$(LIBS_DIR)/golang LD_LIBRARY_PATH=$(LIBS_DIR)/c go build $(BUILDPARAM) godb

test:
	VERBOSE=1 $(MAKE) lib
	GOPATH=$(LIBS_DIR)/golang LD_LIBRARY_PATH=$(LIBS_DIR)/c go test godb
	$(MAKE) clean