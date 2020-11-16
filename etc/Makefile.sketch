# This stub makefile for a Kaleidoscope example pulls in all the targets
# required to build the example

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
SKETCHBOOK_DIR ?= $(HOME)/Documents/Arduino
else
SKETCHBOOK_DIR ?= $(HOME)/Arduino
endif


ifneq ("$(wildcard $(SKETCHBOOK_DIR)/hardware/keyboardio/avr/boards.txt)","")
ARDUINO_DIRECTORIES_USER ?= $(SKETCHBOOK_DIR)
endif

KALEIDOSCOPE_ETC_DIR ?= $(ARDUINO_DIRECTORIES_USER)/hardware/keyboardio/avr/libraries/Kaleidoscope/etc/

include $(KALEIDOSCOPE_ETC_DIR)/makefiles/sketch.mk
