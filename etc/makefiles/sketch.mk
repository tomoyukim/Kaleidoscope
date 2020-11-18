mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

include $(mkfile_dir)/arduino-cli.mk

# Build path config

TMPDIR ?= /tmp



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



SKETCH_BASE_NAME	:=	$(notdir $(sketch_dir))
SKETCH_FILE_NAME	:= 	$(addsuffix .ino, $(SKETCH_BASE_NAME))

sketch_dir_candidates = $(sketch_dir) src/ .
sketch_exists_p = $(realpath $(wildcard $(dir)/$(SKETCH_FILE_NAME)))


export FQBN := $(call _arduino_prop,build.fqbn)



BOOTLOADER_PATH := $(call _arduino_prop,runtime.platform.path)/bootloaders/$(call _arduino_prop,bootloader.file)

# Find the path of the sketch file 
SKETCH_FILE_PATH := $(firstword $(foreach dir,$(sketch_dir_candidates),$(sketch_exists_p)))

# We -could- check to see if sketch-dir is in git before running this command 
# but since we'd just return an empty value in that case, why bother?
GIT_VERSION := $(shell git -C "$(sketch_dir)" describe --abbrev=6 --dirty --alway  2>/dev/null || echo 'unknown')


SKETCH_IDENTIFIER := $(shell echo "${SKETCH_FILE_PATH}" | cksum | cut -d ' ' -f 1)-$(SKETCH_FILE_NAME)





KALEIDOSCOPE_TEMP_PATH ?= $(TMPDIR)/kaleidoscope-$(USER)
KALEIDOSCOPE_BUILD_PATH ?= $(KALEIDOSCOPE_TEMP_PATH)/build
KALEIDOSCOPE_OUTPUT_PATH ?= $(KALEIDOSCOPE_TEMP_PATH)/output

CORE_CACHE_PATH ?= $(KALEIDOSCOPE_TEMP_PATH)/arduino-cores

BUILD_PATH ?= $(KALEIDOSCOPE_BUILD_PATH)/$(SKETCH_IDENTIFIER)
OUTPUT_PATH ?= $(KALEIDOSCOPE_OUTPUT_PATH)/$(SKETCH_IDENTIFIER)


OUTPUT_FILE_PREFIX 		:= $(SKETCH_BASE_NAME)-$(GIT_VERSION)
HEX_FILE_PATH 			:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX).hex
HEX_FILE_WITH_BOOTLOADER_PATH 	:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX)-with-bootloader.hex
ELF_FILE_PATH 			:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX).elf
LIB_FILE_PATH 			:= $(OUTPUT_PATH)/$(OUTPUT_FILE_PREFIX).a


KALEIDOSCOPE_PLATFORM_LIB_DIR := $(abspath (KALEIDOSCOPE_DIR)/..)

# Flashing related config

flashing_instructions	:=	$(call _arduino_prop,build.flashing_instructions)
ifeq ($(flashing_instructions),)
flashing_instruction	:= "If your keyboard needs you to do something to put it in flashing mode, do that now."
endif

port := $(shell $(ARDUINO_CLI) board list --format=text | grep $(FQBN) |cut -d' ' -f 1)


$(SKETCH_FILE_PATH):
	@echo "Sketch is $(SKETCH_FILE_PATH)"


.PHONY: compile configure-arduino-cli install-arduino-core-kaleidoscope install-arduino-core-avr 
.PHONY: disassemble decompile size-map flash clean all test

all: 
	@echo "Make all target doesn't do anything"
	@: ## Do not remove this line, otherwise `make all` will trigger the `%` rule too.


disassemble: ${ELF_FILE_PATH}
	$(call _arduino_prop,compiler.objdump.cmd) \
		$(call _arduino_prop,compiler.objdump.flags) \
		"${ELF_FILE_PATH}"

size-map: ${ELF_FILE_PATH}
	$(call _arduino_prop,compiler.size-map.cmd) \
		$(call _arduino_prop,compiler.size-map.flags) \
		"${ELF_FILE_PATH}"

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

clean:
	rm -rf -- "${OUTPUT_PATH}/*"


compile:
	install -d "${OUTPUT_PATH}"
	@echo "Building ${SKETCH_FILE_PATH}"
	$(ARDUINO_CLI) compile \
		--fqbn "${FQBN}" \
		--libraries "${KALEIDOSCOPE_PLATFORM_LIB_DIR}" \
		--build-path "${BUILD_PATH}" \
		--output-dir "${OUTPUT_PATH}" \
		--build-cache-path "${CORE_CACHE_PATH}" \
		--build-properties "compiler.cpp.extra_flags=${LOCAL_CFLAGS}" \
		--warnings all ${ARDUINO_VERBOSE} ${ccache_wrapper_property} \
		"${SKETCH_FILE_PATH}"
ifeq ($(LIBONLY),)
	@cp "${BUILD_PATH}/${SKETCH_FILE_NAME}.hex" "${HEX_FILE_PATH}"
	@cp "${BUILD_PATH}/${SKETCH_FILE_NAME}.elf" "${ELF_FILE_PATH}"
	@ln -sf "${OUTPUT_FILE_PREFIX}.hex" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest.hex"
	@ln -sf "${OUTPUT_FILE_PREFIX}.elf" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest.elf"
else    
	@cp "${BUILD_PATH}/${SKETCH_FILE_NAME}.a" "${LIB_FILE_PATH}"
	@ln -sf "${OUTPUT_FILE_PREFIX}.a" "${OUTPUT_PATH}/${SKETCH_BASE_NAME}-latest.a"
endif
	$(info Build artifacts can be found in ${BUILD_PATH})


#TODO (arduino team) I'd love to do this with their json output
#but it's short some of the data we kind of need

flash:
ifeq ($(port),)
	$(info Unable to detect keyboard serial port.)
	#@exit 1
endif
	$(info $(flashing_instructions))
	$(info)
	$(info When you're ready to proceed, press 'Enter'.)
	$(info)
	@$(shell read)
	@$(ARDUINO_CLI) upload --fqbn $(FQBN) --port $(port) ${ARDUINO_VERBOSE}
