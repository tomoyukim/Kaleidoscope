# This stub makefile for a Kaleidoscope plugin pulls in 
# all targets from the Kaleidoscope-Plugin library

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
SKETCHBOOK_DIR ?= $(HOME)/Documents/Arduino
else
SKETCHBOOK_DIR ?= $(HOME)/Arduino
endif

ifneq ("$(wildcard $(SKETCHBOOK_DIR)/hardware/keyboardio/avr/boards.txt)","")
ARDUINO_DIRECTORIES_USER ?= $(SKETCHBOOK_DIR)
endif


# If Kaleidoscope's Arduino libraries cannot be found, e.g. because 
# they reside outside of SKETCHBOOK_DIR, we fall back to assuming that 
# the hardware directory can be determined in relation to the position of 
# this Makefile.

KALEIDOSCOPE_ETC_DIR ?= $(ARDUINO_DIRECTORIES_USER)/hardware/keyboardio/avr/libraries/Kaleidoscope/etc/

ifeq ("$(KALEIDOSCOPE_ETC_DIR)/sketch-arduino-cli.mk","")
   # Determine the path of this Makefile
   KALEIDOSCOPE_ETC_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/etc

endif

include $(KALEIDOSCOPE_ETC_DIR)/sketch-arduino-cli.mk

# Set up an argument for passing to the simulator tests in docker
# but if the var isn't set, don't even pass the definition
# since this messes with downstream makefiles

ifneq ($(TEST_PATH),) 
 TEST_PATH_ARG="TEST_PATH='$(TEST_PATH)'"
endif

clean:
	$(MAKE) -C tests clean
	rm -rf testing/googletest/build/*


PLUGIN_TEST_SUPPORT_DIR ?= $(ARDUINO_DIRECTORIES_USER)/hardware/keyboardio/build-tools/
PLUGIN_TEST_BIN_DIR ?= $(PLUGIN_TEST_SUPPORT_DIR)/../toolchain/$(shell gcc --print-multiarch)/bin

prepare-virtual:
	$(MAKE) -C $(ARDUINO_DIRECTORIES_USER)/hardware/keyboardio prepare-virtual


simulator-tests: configure-arduino-cli prepare-virtual
	$(MAKE) -C tests all

docker-simulator-tests:
	${TEST_PATH_ARG} ARDUINO_DIRECTORIES_USER="$(ARDUINO_DIRECTORIES_USER)" ./bin/run-docker "make simulator-tests"

docker-bash:
	ARDUINO_DIRECTORIES_USER="$(ARDUINO_DIRECTORIES_USER)" ./bin/run-docker "bash"

run-tests: prepare-virtual build-gtest-gmock
	$(MAKE) -c tests
	@: # blah

build-gtest-gmock:
	(cd testing/googletest && cmake .)
	$(MAKE) -C testing/googletest

adjust-git-timestamps:
	bin/set-timestamps-from-git

find-filename-conflicts:
	@if [ -d "bin" ]; then \
		bin/find-filename-conflicts; \
	fi

#.PHONY: astyle test cpplint cpplint-noisy shellcheck smoke-examples find-filename-conflicts:

astyle:
	PATH="$(PLUGIN_TEST_BIN_DIR):$(PATH)" $(PLUGIN_TEST_SUPPORT_DIR)/quality/run-astyle

check-astyle: astyle
	PATH="$(PLUGIN_TEST_BIN_DIR):$(PATH)" $(PLUGIN_TEST_SUPPORT_DIR)/quality/astyle-check

cpplint-noisy:
	-$(PLUGIN_TEST_SUPPORT_DIR)/quality/cpplint.py  --filter=-legal/copyright,-build/include,-readability/namespace,-whitespace/line_length,-runtime/references  --recursive --extensions=cpp,h,ino src examples


cpplint:
	$(PLUGIN_TEST_SUPPORT_DIR)/quality/cpplint.py  --quiet --filter=-whitespace,-legal/copyright,-build/include,-readability/namespace,-runtime/references  --recursive --extensions=cpp,h,ino src examples


SHELL_FILES = $(shell if [ -d bin ]; then egrep -n -r -l "(env (ba)?sh)|(/bin/(ba)?sh)" bin; fi)

shellcheck:
	@if [ -d "bin" ]; then \
		shellcheck ${SHELL_FILES}; \
	fi


check-docs:
	doxygen $(PLUGIN_TEST_SUPPORT_DIR)/quality/etc/check-docs.conf 2> /dev/null >/dev/null
	python $(PLUGIN_TEST_SUPPORT_DIR)/quality/doxy-coverage.py /tmp/undocced/xml


SMOKE_SKETCHES=$(shell if [ -d ./examples ]; then find ./examples -type f -name \*ino | xargs -n 1 dirname; fi)

smoke-sketches: $(SMOKE_SKETCHES)
	@echo "Smoke-tested all the sketches"

.PHONY: force

clean: 
	rm -rf -- "testing/googletest/build"
	rm -rf -- "_build"
	@$(KALEIDOSCOPE_BUILDER_DIR)/kaleidoscope-builder clean

$(SMOKE_SKETCHES): force
	$(MAKE) -C $@ -f  $(KALEIDOSCOPE_ETC_DIR)/sketch-arduino-cli.mk compile
