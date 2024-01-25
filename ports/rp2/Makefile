# Makefile for micropython on Raspberry Pi RP2
#
# This is a simple wrapper around cmake

# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to RPI_PICO.
BOARD ?= RPI_PICO
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
ifeq ($(findstring boards/PICO,$(BOARD_DIR)),boards/PICO)
$(warning The PICO* boards have been renamed to RPI_PICO*)
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

$(VERBOSE)MAKESILENT = -s

CMAKE_ARGS = -DMICROPY_BOARD=$(BOARD) -DMICROPY_BOARD_DIR="$(abspath $(BOARD_DIR))"

ifdef USER_C_MODULES
CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

ifneq ($(FROZEN_MANIFEST),)
CMAKE_ARGS += -DMICROPY_FROZEN_MANIFEST=${FROZEN_MANIFEST}
endif

ifeq ($(DEBUG),1)
CMAKE_ARGS += -DCMAKE_BUILD_TYPE=Debug
endif

ifdef BOARD_VARIANT
CMAKE_ARGS += -DMICROPY_BOARD_VARIANT=$(BOARD_VARIANT)
endif

ifdef MICROPY_PREVIEW_VERSION_2
CMAKE_ARGS += -DMICROPY_PREVIEW_VERSION_2=1
endif

HELP_BUILD_ERROR ?= "See \033[1;31mhttps://github.com/micropython/micropython/wiki/Build-Troubleshooting\033[0m"

all:
	[ -e $(BUILD)/Makefile ] || cmake -S . -B $(BUILD) -DPICO_BUILD_DOCS=0 ${CMAKE_ARGS}
	$(MAKE) $(MAKESILENT) -C $(BUILD) || (echo -e $(HELP_BUILD_ERROR); false)

clean:
	$(RM) -rf $(BUILD)

# First ensure that pico-sdk is initialised, then use cmake to pick everything
# else (including board-specific dependencies).
# Running the build with ECHO_SUBMODULES set will trigger py/mkrules.cmake to
# print out the value of the GIT_SUBMODULES variable, prefixed with
# "GIT_SUBMODULES", and then abort. This extracts out that line from the cmake
# output and passes the list of submodules to py/mkrules.mk which does the
# `git submodule init` on each.
submodules:
	$(MAKE) -f ../../py/mkrules.mk GIT_SUBMODULES="lib/pico-sdk" submodules
	@GIT_SUBMODULES=$$(cmake -B $(BUILD)/submodules -DECHO_SUBMODULES=1 ${CMAKE_ARGS} -S . 2>&1 | \
	                  grep '^GIT_SUBMODULES=' | cut -d= -f2); \
	$(MAKE) -f ../../py/mkrules.mk GIT_SUBMODULES="$${GIT_SUBMODULES}" submodules
