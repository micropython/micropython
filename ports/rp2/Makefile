# Makefile for micropython on Raspberry Pi RP2
#
# This is a simple wrapper around cmake

BUILD = build

$(VERBOSE)MAKESILENT = -s

all:
	[ -d $(BUILD) ] || cmake -S . -B $(BUILD) -DPICO_BUILD_DOCS=0
	$(MAKE) $(MAKESILENT) -C $(BUILD)

clean:
	$(RM) -rf $(BUILD)
