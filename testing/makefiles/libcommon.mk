top_dir		:= $(dir $(lastword ${MAKEFILE_LIST}))../..
build_dir := ${top_dir}/_build

LIB_DIR := ${build_dir}/lib
OBJ_DIR := ${build_dir}/obj

CXX_FILES := $(wildcard ${top_dir}/testing/common/*.cpp)
H_FILES		:= $(wildcard ${top_dir}/testing/common/*.h)
BARE_CXX_FILES := $(foreach path,${CXX_FILES},$(notdir ${path}))
OBJ_FILES	:= $(patsubst %.cpp,${OBJ_DIR}/%.o,$(BARE_CXX_FILES))
LIB_FILE	:= libcommon.a

.PHONY: all

all: ${OBJ_FILES} ${LIB_DIR}/${LIB_FILE}

${LIB_DIR}/${LIB_FILE}: ${OBJ_FILES}
	@install -d "${LIB_DIR}"
	ar rcs "${LIB_DIR}/${LIB_FILE}" ${OBJ_FILES}

${OBJ_DIR}/%.o: ${top_dir}/testing/common/%.cpp ${H_FILES}
	@echo "compile $@"
	@install -d "${OBJ_DIR}"
	g++ -o "$@" -c \
		-I${top_dir} \
		-I${top_dir}/src \
		-I${top_dir}/../../../virtual/cores/arduino \
		-I${top_dir}/../Kaleidoscope-HIDAdaptor-KeyboardioHID/src \
		-I${top_dir}/../KeyboardioHID/src \
		-I${top_dir}/testing/googletest/googlemock/include \
		-I${top_dir}/testing/googletest/googletest/include \
		-DARDUINO=10607 \
		-DARDUINO_ARCH_VIRTUAL \
		-DARDUINO_AVR_MODEL01 \
		'-DKALEIDOSCOPE_HARDWARE_H="Kaleidoscope-Hardware-Model01.h"' \
		-DKALEIDOSCOPE_VIRTUAL_BUILD=1 \
		-DKEYBOARDIOHID_BUILD_WITHOUT_HID=1 \
		-DUSBCON=dummy \
		-DARDUINO_ARCH_AVR=1 \
		'-DUSB_PRODUCT="Model 01"' \
		$<

clean:
	rm -rf "${build_dir}"
