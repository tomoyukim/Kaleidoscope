mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

KALEIDOSCOPE_BUILDER_DIR ?= $(abspath $(mkfile_dir)/../bin)

.DEFAULT_GOAL := compile

all: 
	@echo "Make all target doesn't do anything"
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.

decompile: disassemble
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.

disassemble: compile

size-map: compile

flash: compile


%:
	ARDUINO_DIRECTORIES_USER=$(ARDUINO_DIRECTORIES_USER) $(KALEIDOSCOPE_BUILDER_DIR)/kaleidoscope-builder $@

