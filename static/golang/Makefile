BUILDPARAM=

ifeq ($(VERBOSE),1)
BUILDPARAM=-x
endif

GO=go

default_target: all

all:
	$(MAKE) lib

clean:
	GOPATH=$(LIBS_DIR)/golang $(GO) clean godb

lib:
	GOPATH=$(LIBS_DIR)/golang \
	CGO_LDFLAGS="-L${LIBS_DIR}/c/bin -lDB" \
	CGO_CFLAGS="-I${LIBS_DIR}/c/include" \
	LD_LIBRARY_PATH=$(LIBS_DIR)/c/bin \
	$(GO) build $(BUILDPARAM) godb

test:
	VERBOSE=1 $(MAKE) lib
	GOPATH=$(LIBS_DIR)/golang \
	CGO_LDFLAGS="-L${LIBS_DIR}/c/bin -lDB" \
	CGO_CFLAGS="-I${LIBS_DIR}/c/include" \
	LD_LIBRARY_PATH=$(LIBS_DIR)/c/bin \
	$(GO) test godb
	$(MAKE) clean

.PHONY: all lib test clean