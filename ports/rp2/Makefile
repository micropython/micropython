# Makefile for micropython on Raspberry Pi RP2
#
# This is a simple wrapper around cmake

BOARD ?= pico
 
BUILD ?= build-$(BOARD)
BOARD_DIR ?= boards/$(BOARD) 

$(VERBOSE)MAKESILENT = -s

CMAKE_ARGS = -DPICO_BOARD=$(BOARD) -DBOARD_DIR=$(BOARD_DIR)

ifdef USER_C_MODULES
CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

all:
	[ -d $(BUILD) ] || cmake -S . -B $(BUILD) -DPICO_BUILD_DOCS=0 ${CMAKE_ARGS}
	$(MAKE) $(MAKESILENT) -C $(BUILD)

clean:
	$(RM) -rf $(BUILD)
