
BOARD_DIR  = boards/$(BOARD)

include ../../py/mkenv.mk
-include $(BOARD_DIR)/mpconfigboard.mk
-include $(TOP)/lib/mtb-psoc-edge-libs/mtb-makefile.mk

GIT_SUBMODULES += lib/mtb-psoc-edge-libs

all: mtb_build

deploy: mtb_program

port_help:
	@:
	$(info )
	$(info Available commands:)
	$(info ) 
	$(info   make submodules                    Initializes port required submodules)
	$(info   make mtb_init BOARD=<board_name>   Initialized the ModusToolbox libraries for the specified board.)
	$(info	 ..                                 Only needs to be done once before the first build)
	$(info   make BOARD=<board_name>            Build the project for the specified board)
	$(info   make BOARD=<board_name> deploy     Build and deploy the project to the specified board)
	$(info   make help                          Show this help)
	$(info )

help: port_help mtb_help

.PHONY: all deploy help

include $(TOP)/py/mkrules.mk
