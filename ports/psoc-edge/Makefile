
BOARD_DIR  = boards/$(BOARD)

include ../../py/mkenv.mk
-include $(BOARD_DIR)/mpconfigboard.mk
-include $(TOP)/lib/mtb-psoc-edge-libs/mtb-makefile.mk

GIT_SUBMODULES += lib/mtb-psoc-edge-libs

all: mtb_build

.PHONY: all

include $(TOP)/py/mkrules.mk
