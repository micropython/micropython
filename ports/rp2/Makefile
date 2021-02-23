# Makefile for micropython on Raspberry Pi RP2
#
# This is a simple wrapper around cmake

BUILD = build

$(VERBOSE)MAKESILENT = -s

CMAKE_ARGS =

ifdef USER_C_MODULES
CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

all:
	[ -d $(BUILD) ] || cmake -S . -B $(BUILD) -DPICO_BUILD_DOCS=0 ${CMAKE_ARGS}
	$(MAKE) $(MAKESILENT) -C $(BUILD)

clean:
	$(RM) -rf $(BUILD)
