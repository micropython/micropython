# Makefile for MicroPython on Ameba-RTOS.
#
# This is a convenience wrapper around ameba.py (which uses cmake).
# Usage:
#   make BOARD=PKE8721DAF
#   make BOARD=PKE8721DAF deploy PORT=/dev/ttyUSB0

SHELL := /bin/bash

# Select the board to build for.
ifdef BOARD_DIR
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
BOARD ?= PKE8721DAF
BOARD_DIR ?= $(abspath boards/$(BOARD))
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# Build directory reflects the board name.
BUILD ?= build_$(BOARD)

# Read SoC name from the board's mpconfigboard.cmake.
SOC := $(shell sed -n 's/.*AMEBA_BOARD_SOC "\(.*\)".*/\1/p' $(BOARD_DIR)/mpconfigboard.cmake)
ifeq ($(SOC),)
$(error Could not determine SOC from $(BOARD_DIR)/mpconfigboard.cmake)
endif

# The ameba-rtos SDK must be activated before building:
#     source /path/to/ameba-rtos/env.sh
# That exports AMEBA_SDK (the SDK root) and activates the toolchain + venv in
# your shell, which `make` then inherits.  We make no assumption about where
# the SDK lives; its location comes entirely from the sourced environment.
ifndef AMEBA_SDK
$(error AMEBA_SDK is not set; run 'source <ameba-rtos>/env.sh' first)
endif

AMEBA := $(AMEBA_SDK)/ameba.py
RETRY ?= 3

# MicroPython core lives two directories up (in-tree: <micropython>/ports/ameba).
MICROPY_DIR := ../..
# Submodules required to build this port.  mkrules.cmake requires micropython-lib
# whenever a frozen manifest is set, even if the manifest does not use it.
GIT_SUBMODULES = lib/micropython-lib

# deploy requires an explicit serial port.
ifneq ($(filter deploy,$(MAKECMDGOALS)),)
  ifeq ($(PORT),)
    $(error PORT is required for deploy, e.g. make deploy PORT=/dev/ttyUSB0)
  endif
endif

.PHONY: all build pristine clean deploy submodules

all: build

# Initialise the MicroPython submodules required to build this port.
submodules:
	git -C $(MICROPY_DIR) submodule update --init $(GIT_SUBMODULES)

build: submodules _board_sync
	MICROPY_BOARD=$(BOARD) MICROPY_BOARD_DIR=$(BOARD_DIR) \
	python $(AMEBA) build $(SOC) \
	    -D MICROPY_BOARD=$(BOARD) MICROPY_BOARD_DIR=$(BOARD_DIR)
	python makeimg.py \
	    build_$(SOC)/build/.config \
	    build_$(SOC)/boot.bin \
	    build_$(SOC)/app.bin \
	    build_$(SOC)/firmware.bin

pristine: submodules _board_sync
	MICROPY_BOARD=$(BOARD) MICROPY_BOARD_DIR=$(BOARD_DIR) \
	python $(AMEBA) build $(SOC) -p \
	    -D MICROPY_BOARD=$(BOARD) MICROPY_BOARD_DIR=$(BOARD_DIR)
	python makeimg.py \
	    build_$(SOC)/build/.config \
	    build_$(SOC)/boot.bin \
	    build_$(SOC)/app.bin \
	    build_$(SOC)/firmware.bin

# Sync soc_info.json and prj.conf to the selected board before every build.
# This ensures ameba.py always sees the correct SoC even when called directly.
_board_sync:
	@echo '{"soc":{"name":"$(SOC)"}}' > soc_info.json
	@if [ -f $(BOARD_DIR)/prj.conf ]; then cp $(BOARD_DIR)/prj.conf prj.conf; fi

clean:
	python $(AMEBA) clean $(SOC)

deploy:
	@for i in $$(seq 1 $(RETRY)); do \
	  echo ">> flash $$i/$(RETRY) ($(PORT) $(SOC))"; \
	  python $(AMEBA) flash -p $(PORT) $(if $(BAUD),-b $(BAUD)) -dev $(SOC) && exit 0; \
	done; echo ">> flash failed"; exit 1
