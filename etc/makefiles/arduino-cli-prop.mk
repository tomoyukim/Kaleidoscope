mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

# Make a variable lazily evaluated at first call
# From https://blog.jgc.org/2016/07/lazy-gnu-make-variables.html
make-lazy = $(eval $1 = $$(eval $1 := $(value $(1)))$$($1))

system_arduino_cli=$(shell command -v arduino-cli || true)

ifeq ($(system_arduino_cli),) 
export ARDUINO_CLI ?= $(KALEIDOSCOPE_BIN_DIR)/arduino-cli
else
export ARDUINO_CLI ?= $(system_arduino_cli)
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
ifneq ($(FQBN),)
fqbn_arg = --fqbn $(FQBN)
endif

# if we don't have a sketch, make a pretend one so we can run --show properties
# This is because aruduino-cli doesn't currently allow us to get props with
# just an FQBN. We've filed a bug with them
ifeq ($(SKETCH_FILE_PATH),) 
_arudino_props_sketch_arg	=	$(KALEIDOSCOPE_ETC_DIR)/dummy-sketch/
else
_arudino_props_sketch_arg	=	$(SKETCH_FILE_PATH)
endif

_arduino_props = $(shell ${ARDUINO_CLI}  compile $(fqbn_arg) --show-properties "$(_arduino_props_sketch_arg)"|perl -p -e"s/ /🔥/g")
$(call make-lazy,_arduino_props)

_arduino_prop = $(subst $1=,,$(subst 🔥, ,$(filter $1=%,$(_arduino_props))))

# How to use_arduino_prop
# $(call _arduino_prop,recipe.hooks.sketch.prebuild.2.pattern)
