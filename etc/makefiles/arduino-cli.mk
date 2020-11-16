mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

# Make a variable lazily evaluated at first call
# From https://blog.jgc.org/2016/07/lazy-gnu-make-variables.html
make-lazy = $(eval $1 = $$(eval $1 := $(value $(1)))$$($1))

export KALEIDOSCOPE_DIR ?= $(abspath $(mkfile_dir)/../..)
export KALEIDOSCOPE_BIN_DIR ?= $(KALEIDOSCOPE_DIR)/bin

# Arduino CLI config

export ARDUINO_CONTENT ?= $(KALEIDOSCOPE_DIR)/.arduino
export ARDUINO_DIRECTORIES_DATA ?= $(ARDUINO_CONTENT)/data
export ARDUINO_DIRECTORIES_DOWNLOADS ?= $(ARDUINO_CONTENT)/downloads
export ARDUINO_DIRECTORIES_USER ?= $(ARDUINO_CONTENT)/user
export ARDUINO_CLI_CONFIG ?= $(ARDUINO_DIRECTORIES_DATA)/arduino-cli.yaml
export ARDUINO_BOARD_MANAGER_ADDITIONAL_URLS ?= https://raw.githubusercontent.com/keyboardio/boardsmanager/master/package_keyboardio_index.json

system_arduino_cli=$(shell command -v arduino-cli || true)

ifeq ($(system_arduino_cli),) 
export ARDUINO_CLI ?= $(KALEIDOSCOPE_BIN_DIR)/arduino-cli
else
export ARDUINO_CLI ?= $(system_arduino_cli)
endif

ifneq ($(VERBOSE),)
export ARDUINO_VERBOSE ?= --verbose
else
export ARDUINO_VERBOSE ?= 
endif

# This is horrible. But because make doesn't really support
# multi-line variables and we want to cache the full
# _arduino_props in a variable, which means letting make 
# split the properties on space, which is what it converts
# newlines into. To make this go, we we need to replace interior
# spaces in the variables with something. We chose the fire 
# emoji, since it accurately represents our feelings on this 
# state of affairs. Later, when finding props, we need to reverse 
# this process, turning fire into space.
_arduino_props = $(shell ${ARDUINO_CLI}  compile --show-properties "$${SKETCH_FILE_PATH}"|perl -p -e"s/ /🔥/g")
$(call make-lazy,_arduino_props)

_arduino_prop = $(subst $1=,,$(subst 🔥, ,$(filter $1=%,$(_arduino_props))))

# How to use_arduino_prop
# $(call _arduino_prop,recipe.hooks.sketch.prebuild.2.pattern)


#
.PHONY: configure-arduino-cli install-arduino-core-kaleidoscope install-arduino-core-avr 

.DEFAULT_GOAL := all

all: 
	@echo "Make all target doesn't do anything"
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.


install-arduino-cli:
	curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR="$(KALEIDOSCOPE_BIN_DIR)" sh

configure-arduino-cli:
	$(ARDUINO_CLI) config init

install-arduino-core-kaleidoscope:
	$(ARDUINO_CLI) core install "keyboardio:avr"

install-arduino-core-avr: 
	$(ARDUINO_CLI) core install "arduino:avr"

