LIB_DIR?=$(shell pwd)/lib
TEST_DIRS=$(shell dirname $(wildcard */Makefile))

DIRS=../testing/googletest/build ../testing/common ${TEST_DIRS}
CLEANDIRS = $(DIRS:%=clean-%)

.PHONY: subdirs $(CLEANDIRS) cmake-clean

cmake-clean:
	-${MAKE} clean
	rm -rf ../testing/googletest/build

clean: ../testing/googletest/build/Makefile $(CLEANDIRS)
	rm -rf "$(LIB_DIR)"

$(CLEANDIRS):
	cd $(@:clean-%=%) && $(MAKE) clean

all: ${TEST_DIRS}
	@echo TEST_DIRS="${TEST_DIRS}"

googletest: ../testing/googletest/build/Makefile
	cd ../testing/googletest/build && $(MAKE)

../testing/googletest/build/Makefile:
	install -d ../testing/googletest/build && cd ../testing/googletest/build && cmake ..

common:
	cd common && LIB_DIR="${LIB_DIR}" $(MAKE)

Makefile:
	@:

%: common googletest
	cd "$@" && $(MAKE)

.PHONY: googletest common
