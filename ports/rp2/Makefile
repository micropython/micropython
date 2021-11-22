# Makefile for micropython on Raspberry Pi RP2
#
# This is a simple wrapper around cmake

BOARD ?= PICO
 
BUILD ?= build-$(BOARD)
MICROPY_PY_WIZNET5K ?= 5105

$(VERBOSE)MAKESILENT = -s

CMAKE_ARGS = -DMICROPY_BOARD=$(BOARD)
CMAKE_ARGS += -DMICROPY_PY_WIZNET5K=$(MICROPY_PY_WIZNET5K)

ifdef USER_C_MODULES
CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

all:
	[ -d $(BUILD) ] || cmake -S . -B $(BUILD) -DPICO_BUILD_DOCS=0 ${CMAKE_ARGS}
	$(MAKE) $(MAKESILENT) -C $(BUILD)

clean:
	$(RM) -rf $(BUILD)
	$(RM) -rf out.log

GIT_SUBMODULES += lib/pico-sdk lib/tinyusb

submodules:
	$(MAKE) -f ../../py/mkrules.mk GIT_SUBMODULES="$(GIT_SUBMODULES)" submodules
