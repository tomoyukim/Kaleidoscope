mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))


export KALEIDOSCOPE_DIR ?= $(abspath $(mkfile_dir)/..)
export KALEIDOSCOPE_BIN_DIR ?= $(KALEIDOSCOPE_DIR)/bin

# Arduino CLI config

export ARDUINO_CONTENT ?= $(KALEIDOSCOPE_DIR)/.arduino
export ARDUINO_DIRECTORIES_DATA ?= $(ARDUINO_CONTENT)/data
export ARDUINO_DIRECTORIES_DOWNLOADS ?= $(ARDUINO_CONTENT)/downloads
export ARDUINO_DIRECTORIES_USER ?= $(ARDUINO_CONTENT)/user
export ARDUINO_CLI_CONFIG ?= $(ARDUINO_DIRECTORIES_DATA)/arduino-cli.yaml
export ARDUINO_BOARD_MANAGER_ADDITIONAL_URLS ?= https://raw.githubusercontent.com/keyboardio/boardsmanager/master/package_keyboardio_index.json

system_arduino_cli=$(command -v arduino-cli || true)

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

#
# Build path config

TMPDIR ?= /tmp

export KALEIDOSCOPE_TEMP_PATH ?= $(TMPDIR)/kaleidoscope-$(USER)
export KALEIDOSCOPE_BUILD_PATH ?= $(KALEIDOSCOPE_TEMP_PATH)/build
export KALEIDOSCOPE_OUTPUT_PATH ?= $(KALEIDOSCOPE_TEMP_PATH)/output
export CORE_CACHE_PATH ?= $(KALEIDOSCOPE_TEMP_PATH)/arduino-cores


# If the sketch is defined
ifneq ($(SKETCH),) 
# If the sketch isn't a directory, we want to get the directory the sketch is in
ifeq ($(wildcard $(SKETCH)/.),)
sketch_dir	:= $(dir $(SKETCH))
endif
else
# If the sketch wasn't defined as we came in, assume the current directory
# is where we're looking
sketch_dir	:= $(realpath $(CURDIR))
endif



export SKETCH_BASE_NAME	:=	$(notdir $(sketch_dir))
export SKETCH_FILE_NAME	:= 	$(addsuffix .ino, $(SKETCH_BASE_NAME))

sketch_dir_candidates = $(sketch_dir) src/ .
sketch_exists_p = $(realpath $(wildcard $(dir)/$(SKETCH_FILE_NAME)))

# FIND the path of the sketch file 
export SKETCH_FILE_PATH := $(firstword $(foreach dir,$(sketch_dir_candidates),$(sketch_exists_p)))






export SKETCH_IDENTIFIER := $(shell echo "$${SKETCH_FILE_PATH}" | cksum | cut -d ' ' -f 1)-$(SKETCH_FILE_NAME)

export SKETCH_BUILD_DIR := $(SKETCH_IDENTIFIER)/build
export SKETCH_OUTPUT_DIR := $(SKETCH_IDENTIFIER)/output

export BUILD_PATH := $(KALEIDOSCOPE_BUILD_PATH)/$(SKETCH_BUILD_DIR)
export OUTPUT_PATH := $(KALEIDOSCOPE_OUTPUT_PATH)/$(SKETCH_OUTPUT_DIR)

export LIB_PROPERTIES_PATH := "../.."





$(SKETCH_FILE_PATH):
	@echo "Sketch is $(SKETCH_FILE_PATH)"


.DEFAULT_GOAL := compile

all: 
	@echo "Make all target doesn't do anything"
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.


install-arduino-cli:
	curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR="$(KALEIDOSCOPE_BIN_DIR)" sh


install-arduino-core-kaleidoscope:
	$(ARDUINO_CLI) core install "keyboardio:avr"

install-arduino-core-avr: 
	$(ARDUINO_CLI) core install "arduino:avr"

decompile: disassemble
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.

disassemble: compile

size-map: compile

flash: compile


%:
	ARDUINO_DIRECTORIES_USER=$(ARDUINO_DIRECTORIES_USER) $(KALEIDOSCOPE_BIN_DIR)/kaleidoscope-builder $@

