mkfile_dir      := $(dir $(lastword ${MAKEFILE_LIST}))
top_dir         := $(abspath $(mkfile_dir)../..)

include $(mkfile_dir)/shared.mk

build_dir := ${top_dir}/_build

LIB_DIR := ${build_dir}/lib
OBJ_DIR := ${build_dir}/obj

CXX_FILES := $(wildcard ${top_dir}/testing/*.cpp)
H_FILES		:= $(wildcard ${top_dir}/testing/*.h)
BARE_CXX_FILES := $(foreach path,${CXX_FILES},$(notdir ${path}))
OBJ_FILES	:= $(patsubst %.cpp,${OBJ_DIR}/%.o,$(BARE_CXX_FILES))
LIB_FILE	:= libcommon.a

ifneq ($(KALEIDOSCOPE_CCACHE),)
COMPILER_WRAPPER := ccache
endif


.PHONY: all

DEFAULT_GOAL: all

all: ${OBJ_FILES} ${LIB_DIR}/${LIB_FILE}

${LIB_DIR}/${LIB_FILE}: ${OBJ_FILES}
	@install -d "${LIB_DIR}"
	ar rcs "${LIB_DIR}/${LIB_FILE}" ${OBJ_FILES}

${OBJ_DIR}/%.o: ${top_dir}/testing/%.cpp ${H_FILES}
	$(info compile $@)
	@install -d "${OBJ_DIR}"
	$(COMPILER_WRAPPER) g++ -o "$@" -c -std=c++14 ${shared_includes} ${shared_defines} $<

clean:
	rm -rf -- "${build_dir}"
