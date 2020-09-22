LIB_DIR?=$(shell pwd)/lib
TEST_DIRS=$(shell dirname $(wildcard tests/*/Makefile))

DIRS=googletest/build common ${TEST_DIRS}
CLEANDIRS = $(DIRS:%=clean-%)

.PHONY: subdirs $(CLEANDIRS) cmake-clean

cmake-clean:
	-${MAKE} clean
	rm -rf googletest/build

clean: googletest/build/Makefile $(CLEANDIRS)
	rm -rf "$(LIB_DIR)"

$(CLEANDIRS):
	cd $(@:clean-%=%) && $(MAKE) clean

all: ${TEST_DIRS}
	@echo TEST_DIRS="${TEST_DIRS}"

googletest: googletest/build/Makefile
	cd googletest/build && $(MAKE)

googletest/build/Makefile:
	install -d googletest/build && cd googletest/build && cmake ..

common:
	cd common && LIB_DIR="${LIB_DIR}" $(MAKE)

Makefile:
	@:

%: common googletest
	cd "$@" && $(MAKE)

.PHONY: googletest common
