
# get path to this file
MPY_PATH_TO_MTB_ADAPTER_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MPY_DIR_OF_MTB_ADAPTER_MAKEFILE  := $(dir $(MPY_PATH_TO_MTB_ADAPTER_MAKEFILE))

MTB_LIBS_DIR = mtb-libs
MTB_LIBS_BUILD_DIR = mtb-libs/build

# get variable definitions from main makefile
MTB_BASE_EXAMPLE_MAKEFILE_DIR := $(MPY_DIR_OF_MTB_ADAPTER_MAKEFILE)
MPY_MTB_MAIN_MAKEFILE := $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/Makefile
MPY_MTB_TARGET        := $(shell egrep '^ *TARGET' $(MPY_MTB_MAIN_MAKEFILE) | sed 's/^.*= *//g')
MPY_MTB_CONFIG        ?= $(shell egrep '^ *CONFIG' $(MPY_MTB_MAIN_MAKEFILE) | sed 's/^.*= *//g')

MPY_MTB_BOARD_BUILD_DIR        := $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/$(BUILD)
MPY_MTB_BOARD_BUILD_OUTPUT_DIR := $(MPY_MTB_BOARD_BUILD_DIR)/$(MPY_MTB_TARGET)/$(MPY_MTB_CONFIG)

MPY_MTB_LIB_NAME       = $(file < $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/artifact.rsp)

MTB_HOME     ?= $(HOME)/ModusToolbox
OPENOCD_HOME ?= $(MTB_HOME)/tools_3.0/openocd

# $(info MPY_DIR_OF_MTB_ADAPTER_MAKEFILE  : $(MPY_DIR_OF_MTB_ADAPTER_MAKEFILE))

# $(info MPY_MTB_MAIN_MAKEFILE            : $(MPY_MTB_MAIN_MAKEFILE))
$(info MPY_MTB_TARGET                   : $(MPY_MTB_TARGET))
# $(info MPY_MTB_CONFIG                   : $(MPY_MTB_CONFIG))
# $(info MTB_LIB_NAME                     : $(MPY_MTB_LIB_NAME))

$(info MPY_MTB_BOARD_BUILD_DIR          : $(MPY_MTB_BOARD_BUILD_DIR))
$(info MPY_MTB_BOARD_BUILD_OUTPUT_DIR   : $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR))

mtb_init: mtb_add_bsp mtb_set_bsp mtb_get_libs
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
	$(info mtb_deps_dir  : $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)deps/$(MTB_DEPS_DIRS)/)
	$(Q) $(foreach DIR, $(MTB_DEPS_DIRS), $(shell cp -r $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)deps/$(DIR)/. $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)deps))

mtb_get_libs: mtb_config_deps
	$(info )
	$(info Retrieving ModusToolbox dependencies ...)
	$(Q) $(MAKE) -C $(MTB_BASE_EXAMPLE_MAKEFILE_DIR) getlibs

mtb_add_bsp:
	$(info )
	$(info Adding board $(BOARD) dependencies ...)
	$(Q) cd $(MTB_BASE_EXAMPLE_MAKEFILE_DIR); library-manager-cli --project . --add-bsp-name $(BOARD) --add-bsp-version $(BOARD_VERSION)

mtb_set_bsp: 
	$(info )
	$(info Setting board $(BOARD) as active ...)
	$(Q) cd $(MTB_BASE_EXAMPLE_MAKEFILE_DIR); library-manager-cli --project . --set-active-bsp APP_$(BOARD)

# Remove MTB retrieved lib and dependencies
mtb_deinit: clean
	$(info )
	$(info Removing mtb_shared and libs folder ...)
	-$(Q) cd $(MTB_BASE_EXAMPLE_MAKEFILE_DIR); rm -rf libs
	-$(Q) cd $(MTB_BASE_EXAMPLE_MAKEFILE_DIR); rm -rf bsps
	-$(Q) cd $(MTB_BASE_EXAMPLE_MAKEFILE_DIR); rm -rf ../mtb_shared
	-$(Q) cd $(MTB_BASE_EXAMPLE_MAKEFILE_DIR); find deps/ -maxdepth 1 -type f -delete 

# Some of the configuration variables are passed to the ModusToolbox 
# Makefile to include/exclude certain middleware libraries and components
MPY_MTB_MAKE_VARS = MICROPY_PY_NETWORK=$(MICROPY_PY_NETWORK)

# build MTB project
mtb_build:
	$(info )
	$(info Building $(BOARD) in $(CONFIG) mode using MTB ...)
	$(Q) $(MAKE) -C $(MTB_BASE_EXAMPLE_MAKEFILE_DIR) CONFIG=$(MPY_MTB_CONFIG) $(MPY_MTB_MAKE_VARS) build 


mtb_clean:
	-$ rm -rf $(MTB_LIBS_BUILD_DIR)

# get variables set for includes, objects, etc. and add to mpy makefile variables
mtb_get_build_flags: mtb_build
	@:
	$(eval MPY_MTB_INCLUDE_DIRS = $(file < $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/inclist.rsp))
	$(eval INC += $(subst -I,-I$(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/,$(MPY_MTB_INCLUDE_DIRS)))
	$(eval INC += -I$(BOARD_DIR))
	$(eval MPY_MTB_LIBRARIES = $(file < $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/liblist.rsp))
	$(eval LIBS += $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/$(MPY_MTB_LIB_NAME))
#	$(eval LIBS += $(MPY_MTB_LIBRARIES) $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/$(MPY_MTB_LIB_NAME))
# TODO: hardcode this flags in a board a separate board folder or in the general board folder
# python mtb_build_info.py ccxxflags build/APP_CY8CPROTO-062-4343W/Debug/.cycompiler
#	$(eval CFLAGS_HARCODED =  -mcpu=cortex-m4 --specs=nano.specs -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb -ffunction-sections -fdata-sections -ffat-lto-objects -g -Wall -pipe -DCOMPONENT_4343W -DCOMPONENT_APP_CY8CPROTO_062_4343W -DCOMPONENT_CAT1 -DCOMPONENT_CAT1A -DCOMPONENT_CM0P_SLEEP -DCOMPONENT_CM4 -DCOMPONENT_CM4_0 -DCOMPONENT_Debug -DCOMPONENT_GCC_ARM -DCOMPONENT_HCI_UART -DCOMPONENT_MURATA_1DX -DCOMPONENT_MW_ABSTRACTION_RTOS -DCOMPONENT_MW_CAT1CM0P -DCOMPONENT_MW_CLIB_SUPPORT -DCOMPONENT_MW_CMSIS -DCOMPONENT_MW_CONNECTIVITY_UTILITIES -DCOMPONENT_MW_CORE_LIB -DCOMPONENT_MW_CORE_MAKE -DCOMPONENT_MW_CY_MBEDTLS_ACCELERATION -DCOMPONENT_MW_FREERTOS -DCOMPONENT_MW_LWIP -DCOMPONENT_MW_LWIP_FREERTOS_INTEGRATION -DCOMPONENT_MW_LWIP_NETWORK_INTERFACE_INTEGRATION -DCOMPONENT_MW_MBEDTLS -DCOMPONENT_MW_MTB_HAL_CAT1 -DCOMPONENT_MW_MTB_PDL_CAT1 -DCOMPONENT_MW_RECIPE_MAKE_CAT1A -DCOMPONENT_MW_RETARGET_IO -DCOMPONENT_MW_SECURE_SOCKETS -DCOMPONENT_MW_SERIAL_FLASH -DCOMPONENT_MW_WHD_BSP_INTEGRATION -DCOMPONENT_MW_WIFI_CONNECTION_MANAGER -DCOMPONENT_MW_WIFI_CORE_FREERTOS_LWIP_MBEDTLS -DCOMPONENT_MW_WIFI_HOST_DRIVER -DCOMPONENT_MW_WPA3_EXTERNAL_SUPPLICANT -DCOMPONENT_PSOC6_02 -DCOMPONENT_SOFTFP -DCOMPONENT_WIFI_INTERFACE_SDIO -DCORE_NAME_CM4_0=1 -DCY8C624ABZI_S2D44 -DCYBSP_WIFI_CAPABLE -DCY_APPNAME_mtb_example_wifi_scan -DCY_RETARGET_IO_CONVERT_LF_TO_CRLF -DCY_RTOS_AWARE -DCY_SUPPORTS_DEVICE_VALIDATION -DCY_TARGET_BOARD=APP_CY8CPROTO_062_4343W -DCY_USING_HAL -DCY_WIFI_HOST_WAKE_SW_FORCE=0 -DDEBUG -DMBEDTLS_USER_CONFIG_FILE=mbedtls_user_config.h -DTARGET_APP_CY8CPROTO_062_4343W  -DCOMPONENT_FREERTOS  -DCOMPONENT_LWIP -DCOMPONENT_MBEDTLS)

# Mismatch between flags (CY8CPROTO-062-4343W):
# harcoded:
# (+)-DCOMPONENT_MW_WIFI_CORE_FREERTOS_LWIP_MBEDTLS 
# (+)-DCY_APPNAME_mtb_example_wifi_scan
# (+)-DMBEDTLS_USER_CONFIG_FILE=mbedtls_user_config.h
# from script:
# (+)-DCY_LIBNAME_libmtb 
# (+)-DMBEDTLS_USER_CONFIG_FILE=mbedtls_config.h
	$(eval CFLAGS_PYTHON += $(shell $(PYTHON) $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/mtb_build_info.py ccxxflags $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/.cycompiler ))
	$(eval CFLAGS += $(CFLAGS_PYTHON))
#  $(info CFLAGS ::: $(CFLAGS_HARCODED))
	$(info CFLAGS_ ::: $(CFLAGS_PYTHON))
	$(eval CXXFLAGS += $(CFLAGS))
# python mtb_build_info.py ldflags build/APP_CY8CPROTO-062-4343W/Debug/.cylinker
	$(eval LDFLAGS_PY = $(shell $(PYTHON) $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/mtb_build_info.py ldflags $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/.cylinker $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)))
	$(info LDFLAGS_PY ::: $(LDFLAGS_PY))
# $(eval LDFLAGS += -mcpu=cortex-m4 --specs=nano.specs -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb -ffunction-sections -fdata-sections -ffat-lto-objects -g -Wall -pipe -Wl,--gc-sections -T$(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/bsps/TARGET_APP_CY8CPROTO-062-4343W/COMPONENT_CM4/TOOLCHAIN_GCC_ARM/linker.ld -Wl,-Map,$(BUILD)/firmware.map -Wl,--start-group -Wl,--end-group -Wl,--print-memory-usage)
	$(eval LDFLAGS += $(LDFLAGS_PY))
# $(eval LDFLAGS += -Wl,-Map,$(BUILD)/firmware.map -Wl,--start-group -Wl,--end-group -Wl,--print-memory-usage)
	$(eval QSTR_GEN_CFLAGS += $(INC) $(CFLAGS))

attached_devs:
	@:
	$(eval ATTACHED_TARGET_LIST = $(shell fw-loader --device-list | sed -n -e 's/[0-9]: KitProg3 CMSIS-DAP BULK-//' -e 's/FW Version[^0-9]*\(\([0-9]\.\)\{0,4\}[0-9][^.]\).*//p'| sed -n 's/^[ \t]*//p'))
	$(eval ATTACHED_TARGETS_NUMBER = $(words $(ATTACHED_TARGET_LIST)))
	$(info Number of attached targets : $(ATTACHED_TARGETS_NUMBER))
	$(info List of attached targets : $(ATTACHED_TARGET_LIST))

ifndef EXT_HEX_FILE
HEX_FILE = $(MPY_DIR_OF_MAIN_MAKEFILE)/build/firmware.hex
PROG_DEPS=build
else
HEX_FILE = $(EXT_HEX_FILE)
endif

program: $(PROG_DEPS)
	@:
	$(info )
	$(info Programming using openocd ...)
	openocd -s $(OPENOCD_HOME)/scripts -s $(MTB_BASE_EXAMPLE_MAKEFILE_DIR)/bsps/TARGET_APP_$(BOARD)/config/GeneratedSource -c "source [find interface/kitprog3.cfg]; $(SERIAL_ADAPTER_CMD) ; source [find target/psoc6_2m.cfg]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program $(HEX_FILE) verify reset exit;"
	$(info Programming done.)

program_multi: attached_devs
	@:
	$(foreach ATTACHED_TARGET, $(ATTACHED_TARGET_LIST), $(MAKE) program SERIAL_ADAPTER_CMD='adapter serial $(ATTACHED_TARGET)';)


.PHONY: mtb_init mtb_deinit mtb_config_deps mtb_build mtb_get_build_flags program program_multi