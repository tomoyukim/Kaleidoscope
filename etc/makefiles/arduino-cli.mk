mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

include $(mkfile_dir)/arduino-cli-prop.mk


.PHONY: configure-arduino-cli install-arduino-core-kaleidoscope install-arduino-core-avr
.PHONY: all

.DEFAULT_GOAL := non-goal

non-goal: 
	@echo "Make without a target doesn't do anything"
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.


install-arduino-cli:
	curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR="$(KALEIDOSCOPE_BIN_DIR)" sh

configure-arduino-cli:
	$(ARDUINO_CLI) config init

install-arduino-core-kaleidoscope:
	$(ARDUINO_CLI) core install "keyboardio:avr"

install-arduino-core-avr: 
	$(ARDUINO_CLI) core install "arduino:avr"

