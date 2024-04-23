# get variable definitions from main makefile
MTB_LIBS_DIR       = mtb-libs
MTB_MAIN_MAKEFILE := $(MTB_LIBS_DIR)/Makefile
MTB_CONFIG        ?= $(shell egrep '^ *CONFIG' $(MTB_MAIN_MAKEFILE) | sed 's/^.*= *//g')

MTB_LIBS_BUILD_DIR       := $(MTB_LIBS_DIR)/$(BUILD)
MTB_LIBS_BOARD_BUILD_DIR := $(MTB_LIBS_BUILD_DIR)/APP_$(BOARD)/$(MPY_MTB_CONFIG)

MTB_STATIC_LIB_NAME          = $(file < $(MTB_LIBS_BOARD_BUILD_DIR)/artifact.rsp)

$(info MTB_MAIN_MAKEFILE                : $(MTB_MAIN_MAKEFILE))
$(info MTB_TARGET                       : APP_$(BOARD))
$(info MTB_CONFIG                       : $(MTB_CONFIG))
$(info MTB_LIB_NAME                     : $(MTB_STATIC_LIB_NAME))

$(info MTB_LIBS_BUILD_DIR               : $(MTB_LIBS_BUILD_DIR))
$(info MTB_LIBS_BOARD_BUILD_DIR         : $(MTB_LIBS_BOARD_BUILD_DIR))

mtb_init: mtb_deinit mtb_add_bsp mtb_set_bsp mtb_get_libs
	$(info )
	$(info Initializing ModusToolbox libs for board $(BOARD))

# Choose the ModusToolbox deps according to board config parameters
MTB_DEPS_DIRS = common
ifeq ($(MICROPY_PY_NETWORK),1)
MTB_DEPS_DIRS += network
endif

ifeq ($(MICROPY_PY_EXT_FLASH),1)
MTB_DEPS_DIRS += ext_flash
endif

ifeq ($(MICROPY_PY_SSL), 1)
MTB_DEPS_DIRS += crypto
endif

# The ModusToolbox expects all the .mtb files to be in the /deps folder.
# The feature specific dependencies organized in folders are directly copied 
# to the deps/ root folder
# In theory, the build inclusion/exclusion of components can be handled by the 
# COMPONENTS variable of the ModusToolbox Makefile. T
# This feature does not seem to scale well for this use case (Or better knowledge
# on how to use it is required :|).
# It seems easier to just explicitly include only those middleware assets 
# and libraries required for a given bsp and its required MicroPython capabilities.
mtb_config_deps:
	@:
	$(info )
	$(info Configuring ModusToolbox dependencies ...)
	$(info mtb_deps_dir  : $(MTB_LIBS_DIR)/deps/$(MTB_DEPS_DIRS)/)
	$(Q) $(foreach DIR, $(MTB_DEPS_DIRS), $(shell cp -r $(MTB_LIBS_DIR)/deps/$(DIR)/. $(MTB_LIBS_DIR)/deps))

mtb_get_libs: mtb_config_deps
	$(info )
	$(info Retrieving ModusToolbox dependencies ...)
	$(Q) $(MAKE) -C $(MTB_LIBS_DIR) getlibs

mtb_add_bsp:
	$(info )
	$(info Adding board $(BOARD) dependencies ...)
	$(Q) cd $(MTB_LIBS_DIR); library-manager-cli --project . --add-bsp-name $(BOARD) --add-bsp-version $(BOARD_VERSION)

mtb_set_bsp: 
	$(info )
	$(info Setting board $(BOARD) as active ...)
	$(Q) cd $(MTB_LIBS_DIR); library-manager-cli --project . --set-active-bsp APP_$(BOARD)

# Remove MTB retrieved lib and dependencies
mtb_deinit: clean
	$(info )
	$(info Removing mtb_shared and libs folder ...)
	-$(Q) cd $(MTB_LIBS_DIR); rm -rf libs
	-$(Q) cd $(MTB_LIBS_DIR); rm -rf bsps
	-$(Q) cd $(MTB_LIBS_DIR); rm -rf ../mtb_shared
	-$(Q) cd $(MTB_LIBS_DIR); find deps/*.mtb -maxdepth 1 -type f -delete 

# Some of the configuration variables are passed to the ModusToolbox 
# Makefile to include/exclude certain middleware libraries and components
MPY_MTB_MAKE_VARS = MICROPY_PY_NETWORK=$(MICROPY_PY_NETWORK) MICROPY_PY_SSL=$(MICROPY_PY_SSL) BOARD=$(BOARD)

# build MTB project
mtb_build:
	$(info )
	$(info Building $(BOARD) in $(CONFIG) mode using MTB ...)
	$(Q) $(MAKE) -C $(MTB_LIBS_DIR) CONFIG=$(MPY_MTB_CONFIG) $(MPY_MTB_MAKE_VARS) build 


mtb_clean:
	-$ rm -rf $(MTB_LIBS_BUILD_DIR)

# get variables set for includes, objects, etc. and add to mpy makefile variables
mtb_get_build_flags: mtb_build
	@:
	$(eval MPY_MTB_INCLUDE_DIRS = $(file < $(MTB_LIBS_BOARD_BUILD_DIR)/inclist.rsp))
	$(eval INC += $(subst -I,-I$(MTB_LIBS_DIR)/,$(MPY_MTB_INCLUDE_DIRS)))
	$(eval INC += -I$(BOARD_DIR))
	$(eval MPY_MTB_LIBRARIES = $(file < $(MTB_LIBS_BOARD_BUILD_DIR)/liblist.rsp))
	$(eval LIBS += $(MTB_LIBS_BOARD_BUILD_DIR)/$(MTB_STATIC_LIB_NAME))
	$(eval CFLAGS += $(shell $(PYTHON) $(MTB_LIBS_DIR)/mtb_build_info.py ccxxflags $(MTB_LIBS_BOARD_BUILD_DIR)/.cycompiler ))
	$(eval CXXFLAGS += $(CFLAGS))
	$(eval LDFLAGS += $(shell $(PYTHON) $(MTB_LIBS_DIR)/mtb_build_info.py ldflags $(MTB_LIBS_BOARD_BUILD_DIR)/.cylinker $(MTB_LIBS_DIR)))
	$(eval QSTR_GEN_CFLAGS += $(INC) $(CFLAGS))

# When multiple types of boards are connected, a devs file needs to be provided.
# When working locally, if a "local-devs.yml" file is placed in "tools/psoc6"
# it will be used
ifneq ($(DEVS_FILE),)
MULTI_BOARD_DEVS_OPTS = -b $(BOARD) -y $(DEVS_FILE)
else 
DFLT_LOCAL_DEVS_FILE_NAME = local-devs.yml
LOCAL_DEVS_FILE=$(TOP)/tools/psoc6/$(DFLT_LOCAL_DEVS_FILE_NAME)
ifneq (,$(wildcard $(LOCAL_DEVS_FILE)))
MULTI_BOARD_DEVS_OPTS = -b $(BOARD) -y $(LOCAL_DEVS_FILE)
endif
endif

attached_devs:
	@:
	$(eval ATTACHED_TARGET_LIST = $(shell $(PYTHON) $(TOP)/tools/psoc6/get-devs.py serial-number $(MULTI_BOARD_DEVS_OPTS)))
	$(eval ATTACHED_TARGETS_NUMBER = $(words $(ATTACHED_TARGET_LIST)))
	$(info Number of attached targets : $(ATTACHED_TARGETS_NUMBER))
	$(info List of attached targets : $(ATTACHED_TARGET_LIST))

ifndef EXT_HEX_FILE
HEX_FILE = $(BUILD)/firmware.hex
PROG_DEPS=build
else
HEX_FILE = $(EXT_HEX_FILE)
endif

MTB_HOME     ?= $(HOME)/ModusToolbox
OPENOCD_HOME ?= $(MTB_HOME)/tools_3.0/openocd

# Selection of openocd cfg files based on board
OPENOCD_CFG_SEARCH = $(MTB_LIBS_DIR)/bsps/TARGET_APP_$(BOARD)/config/GeneratedSource

ifeq ($(BOARD),CY8CPROTO-062-4343W)
OPENOCD_TARGET_CFG=psoc6_2m.cfg
else ifeq ($(BOARD),CY8CPROTO-063-BLE)
OPENOCD_TARGET_CFG=psoc6.cfg
endif

program: $(PROG_DEPS)
	@:
	$(info )
	$(info Programming using openocd ...)
	openocd -s $(OPENOCD_HOME)/scripts -s $(OPENOCD_CFG_SEARCH) -c "source [find interface/kitprog3.cfg]; $(SERIAL_ADAPTER_CMD) ; source [find target/$(OPENOCD_TARGET_CFG)]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program $(HEX_FILE) verify reset exit;"
	$(info Programming done.)

program_multi: attached_devs
	@:
	$(foreach ATTACHED_TARGET, $(ATTACHED_TARGET_LIST), $(MAKE) program SERIAL_ADAPTER_CMD='adapter serial $(ATTACHED_TARGET)';)


.PHONY: mtb_init mtb_deinit mtb_config_deps mtb_build mtb_get_build_flags program program_multi