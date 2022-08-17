# Makefile for micropython on Raspberry Pi RP2
#
# This is a simple wrapper around cmake

BOARD ?= PICO
 
BUILD ?= build-$(BOARD)

$(VERBOSE)MAKESILENT = -s

CMAKE_ARGS = -DMICROPY_BOARD=$(BOARD)

ifdef USER_C_MODULES
CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

ifneq ($(FROZEN_MANIFEST),)
CMAKE_ARGS += -DMICROPY_FROZEN_MANIFEST=${FROZEN_MANIFEST}
endif

ifeq ($(DEBUG),1)
CMAKE_ARGS += -DCMAKE_BUILD_TYPE=Debug
endif

all:
	[ -e $(BUILD)/Makefile ] || cmake -S . -B $(BUILD) -DPICO_BUILD_DOCS=0 ${CMAKE_ARGS}
	$(MAKE) $(MAKESILENT) -C $(BUILD)

clean:
	$(RM) -rf $(BUILD)

# First ensure that pico-sdk is initialised, then use cmake to pick everything
# else (including board-specific dependencies).
submodules:
	$(MAKE) -f ../../py/mkrules.mk GIT_SUBMODULES="lib/pico-sdk" submodules
	GIT_SUBMODULES=$$(cmake -B $(BUILD)/submodules -DECHO_SUBMODULES=1 ${CMAKE_ARGS} -S . 2>&1 | \
	                  grep '^GIT_SUBMODULES=' | cut -d= -f2); \
	$(MAKE) -f ../../py/mkrules.mk GIT_SUBMODULES="$${GIT_SUBMODULES}" submodules
