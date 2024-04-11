# Makefile for MicroPython on ESP32.
#
# This is a simple, convenience wrapper around idf.py (which uses cmake).

# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to ESP32_GENERIC.
BOARD ?= ESP32_GENERIC
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
ifeq ($(findstring boards/GENERIC,$(BOARD_DIR)),boards/GENERIC)
$(warning The GENERIC* boards have been renamed to ESP32_GENERIC*)
endif
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# If the build directory is not given, make it reflect the board name (and
# optionally the board variant).
ifneq ($(BOARD_VARIANT),)
BUILD ?= build-$(BOARD)-$(BOARD_VARIANT)
else
BUILD ?= build-$(BOARD)
endif

# Device serial settings.
PORT ?= /dev/ttyUSB0
BAUD ?= 460800

PYTHON ?= python3

.PHONY: all clean deploy erase submodules FORCE

CMAKE_ARGS =

ifdef USER_C_MODULES
	CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

IDFPY_FLAGS += -D MICROPY_BOARD=$(BOARD) -D MICROPY_BOARD_DIR="$(abspath $(BOARD_DIR))" $(CMAKE_ARGS)

ifdef FROZEN_MANIFEST
       IDFPY_FLAGS += -D MICROPY_FROZEN_MANIFEST=$(FROZEN_MANIFEST)
endif

ifdef BOARD_VARIANT
	IDFPY_FLAGS += -D MICROPY_BOARD_VARIANT=$(BOARD_VARIANT)
endif

ifdef MICROPY_PREVIEW_VERSION_2
	IDFPY_FLAGS += -D MICROPY_PREVIEW_VERSION_2=1
endif

HELP_BUILD_ERROR ?= "See \033[1;31mhttps://github.com/micropython/micropython/wiki/Build-Troubleshooting\033[0m"

define RUN_IDF_PY
	idf.py $(IDFPY_FLAGS) -B $(BUILD) -p $(PORT) -b $(BAUD) $(1)
endef

all:
	idf.py $(IDFPY_FLAGS) -B $(BUILD) build || (echo -e $(HELP_BUILD_ERROR); false)
	@$(PYTHON) makeimg.py \
		$(BUILD)/sdkconfig \
		$(BUILD)/bootloader/bootloader.bin \
		$(BUILD)/partition_table/partition-table.bin \
		$(BUILD)/micropython.bin \
		$(BUILD)/firmware.bin \
		$(BUILD)/micropython.uf2

$(BUILD)/bootloader/bootloader.bin $(BUILD)/partition_table/partition-table.bin $(BUILD)/micropython.bin: FORCE

clean:
	$(call RUN_IDF_PY,fullclean)

deploy:
	$(call RUN_IDF_PY,flash)

erase:
	$(call RUN_IDF_PY,erase-flash)

monitor:
	$(call RUN_IDF_PY,monitor)

size:
	$(call RUN_IDF_PY,size)

size-components:
	$(call RUN_IDF_PY,size-components)

size-files:
	$(call RUN_IDF_PY,size-files)

# Running the build with ECHO_SUBMODULES set will trigger py/mkrules.cmake to
# print out the value of the GIT_SUBMODULES variable, prefixed with
# "GIT_SUBMODULES", and then abort. This extracts out that line from the idf.py
# output and passes the list of submodules to py/mkrules.mk which does the
# `git submodule init` on each.
submodules:
	@GIT_SUBMODULES=$$(idf.py $(IDFPY_FLAGS) -B $(BUILD)/submodules -D ECHO_SUBMODULES=1 build 2>&1 | \
	                  grep '^GIT_SUBMODULES=' | cut -d= -f2); \
	$(MAKE) -f ../../py/mkrules.mk GIT_SUBMODULES="$${GIT_SUBMODULES}" submodules
