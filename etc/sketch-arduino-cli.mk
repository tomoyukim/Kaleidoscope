mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))


# Make a variable lazily evaluated at first call
# From https://blog.jgc.org/2016/07/lazy-gnu-make-variables.html
make-lazy = $(eval $1 = $$(eval $1 := $(value $(1)))$$($1))






export KALEIDOSCOPE_DIR ?= $(abspath $(mkfile_dir)/..)
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


export BOOTLOADER_PATH := $(call _arduino_prop,runtime.platform.path)/bootloaders/$(call _arduino_prop,bootloader.file)

# Find the path of the sketch file 
export SKETCH_FILE_PATH := $(firstword $(foreach dir,$(sketch_dir_candidates),$(sketch_exists_p)))

# We -could- check to see if sketch-dir is in git before running this command 
# but since we'd just return an empty value in that case, why bother?
export GIT_VERSION := $(shell git -C "$(sketch_dir)" describe --abbrev=6 --dirty --alway  2>/dev/null || echo 'unknown')


export SKETCH_IDENTIFIER := $(shell echo "$${SKETCH_FILE_PATH}" | cksum | cut -d ' ' -f 1)-$(SKETCH_FILE_NAME)

export SKETCH_BUILD_DIR ?= $(SKETCH_IDENTIFIER)/build
export SKETCH_OUTPUT_DIR ?= $(SKETCH_IDENTIFIER)/output

export BUILD_PATH ?= $(KALEIDOSCOPE_BUILD_PATH)/$(SKETCH_BUILD_DIR)
export OUTPUT_PATH ?= $(KALEIDOSCOPE_OUTPUT_PATH)/$(SKETCH_OUTPUT_DIR)


export OUTPUT_FILE_PREFIX 		:= $(SKETCH_BASE_NAME)-$(GIT_VERSION)
export HEX_FILE_PATH 			:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX).hex
export HEX_FILE_WITH_BOOTLOADER_PATH 	:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX)-with-bootloader.hex
export ELF_FILE_PATH 			:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX).elf
export LIB_FILE_PATH 			:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX).a



export LIB_PROPERTIES_PATH := "../.."


# We should use compiler.path instead of appending bin, but we 
# don't have substitution for arduino props yet

export COMPILER_PATH	:=	$(call _arduino_prop,runtime.tools.avr-gcc.path)/bin

# Allow the compiler prefix to be empty for virtual builds
COMPILER_PREFIX 	?= 	avr-
AVR_OBJDUMP		:=	${COMPILER_PATH}/${COMPILER_PREFIX}objdump
AVR_OBJCOPY		:=	${COMPILER_PATH}/${COMPILER_PREFIX}objcopy
AVR_NM			:=	${COMPILER_PATH}/${COMPILER_PREFIX}nm
AVR_SIZE		:=	${COMPILER_PATH}/${COMPILER_PREFIX}size

$(SKETCH_FILE_PATH):
	@echo "Sketch is $(SKETCH_FILE_PATH)"


.PHONY: compile configure-arduino-cli install-arduino-core-kaleidoscope install-arduino-core-avr 
.PHONY: disassemble decompile size-map flash clean

.DEFAULT_GOAL := compile

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

decompile: disassemble
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.

disassemble: ${ELF_FILE_PATH}
	${AVR_OBJDUMP} -C -d "${ELF_FILE_PATH}"

size-map: ${ELF_FILE_PATH}
	${AVR_NM} --size-sort -C -r -l -t decimal "${ELF_FILE_PATH}"

flash: ${HEX_FILE_PATH}

${ELF_FILE_PATH}: compile
${HEX_FILE_PATH}: compile
	

hex-with-bootloader: ${HEX_FILE_PATH}  
	awk '/^:00000001FF/ == 0' "${HEX_FILE_PATH}" >"${HEX_FILE_WITH_BOOTLOADER_PATH}"
	@echo "Using ${BOOTLOADER_PATH}"
	cat "${BOOTLOADER_PATH}" >>"${HEX_FILE_WITH_BOOTLOADER_PATH}"
	ln -sf -- "${OUTPUT_FILE_PREFIX}-with-bootloader.hex" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest-with-bootloader.hex"
	@echo Combined firmware and bootloader are now at 
	@echo ${HEX_FILE_WITH_BOOTLOADER_PATH}
	@echo
	@echo Make sure you have the bootloader version you expect.
	@echo
	@echo
	@echo And TEST THIS ON REAL HARDWARE BEFORE YOU GIVE IT TO ANYONE.




prop:
	@echo $(BOOTLOADER_PATH)
	@echo $(GIT_VERSION)

clean:
	rm -rf -- "${OUTPUT_PATH}/*"


_do_compile:

compile:
	install -d "${OUTPUT_PATH}"
	@echo "Building ${SKETCH_FILE_PATH}"
	$(ARDUINO_CLI) compile \
		--fqbn "${FQBN}" \
		--libraries "${KALEIDOSCOPE_DIR}/.." \
		--build-path "${BUILD_PATH}" \
		--output-dir "${OUTPUT_PATH}" \
		--build-cache-path "${CORE_CACHE_PATH}" \
		--build-properties "compiler.cpp.extra_flags=${LOCAL_CFLAGS}" \
		--warnings all ${ARDUINO_VERBOSE} \
		"${SKETCH_FILE_PATH}"
ifeq ($(LIBONLY),)
	cp "${BUILD_PATH}/${SKETCH_FILE_NAME}.hex" "${HEX_FILE_PATH}"
	cp "${BUILD_PATH}/${SKETCH_FILE_NAME}.elf" "${ELF_FILE_PATH}"
	ln -sf "${OUTPUT_FILE_PREFIX}.hex" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest.hex"
	ln -sf "${OUTPUT_FILE_PREFIX}.elf" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest.elf"
else    
	cp "${BUILD_PATH}/${SKETCH_FILE_NAME}.a" "${LIB_FILE_PATH}"
	ln -sf "${OUTPUT_FILE_PREFIX}.a" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest.a"
endif
	@echo "Build artifacts can be found in ${BUILD_PATH}"



%:
	$(KALEIDOSCOPE_BIN_DIR)/kaleidoscope-builder $@

