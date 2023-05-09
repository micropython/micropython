
# get path to this file
MPY_PATH_TO_MTB_ADAPTER_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MPY_DIR_OF_MTB_ADAPTER_MAKEFILE  := $(dir $(MPY_PATH_TO_MTB_ADAPTER_MAKEFILE))


# get variable definitions from main makefile
MPY_MTB_MAIN_MAKEFILE := $(MPY_DIR_OF_MTB_ADAPTER_MAKEFILE)/Makefile
MPY_MTB_TARGET        := $(shell egrep '^ *TARGET' $(MPY_MTB_MAIN_MAKEFILE) | sed 's/^.*= *//g')
MPY_MTB_CONFIG        ?= $(shell egrep '^ *CONFIG' $(MPY_MTB_MAIN_MAKEFILE) | sed 's/^.*= *//g')

MPY_MTB_BOARD_BUILD_DIR        := $(MPY_DIR_OF_MTB_ADAPTER_MAKEFILE)/$(BUILD)
MPY_MTB_BOARD_BUILD_OUTPUT_DIR := $(MPY_MTB_BOARD_BUILD_DIR)/$(MPY_MTB_TARGET)/$(MPY_MTB_CONFIG)

MPY_MTB_LIB_NAME       = $(file < $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/artifact.rsp)



# using WSL : should be something like /mnt/c/Users/???/ModusToolbox  with ??? to be replaced by user windows home directory
MTB_WIN_HOME ?= $(OLDPWD)/ModusToolbox
MTB_HOME     ?= $(HOME)/ModusToolbox

OPENOCD_HOME     ?= $(MTB_HOME)/tools_3.0/openocd
OPENOCD_WIN_HOME ?= $(MTB_WIN_HOME)/tools_3.0/openocd


$(info MPY_DIR_OF_MTB_ADAPTER_MAKEFILE  : $(MPY_DIR_OF_MTB_ADAPTER_MAKEFILE))

$(info MPY_MTB_MAIN_MAKEFILE            : $(MPY_MTB_MAIN_MAKEFILE))
$(info MPY_MTB_TARGET                   : $(MPY_MTB_TARGET))
$(info MPY_MTB_CONFIG                   : $(MPY_MTB_CONFIG))
$(info MTB_LIB_NAME                     : $(MPY_MTB_LIB_NAME))

$(info MPY_MTB_BOARD_BUILD_DIR          : $(MPY_MTB_BOARD_BUILD_DIR))
$(info MPY_MTB_BOARD_BUILD_OUTPUT_DIR   : $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR))


# init MTB project
mpy_mtb_init:
	$(info )
	$(info Initializing $(BOARD) ...)
	$(Q) cd $(BOARD_DIR); $(MAKE) getlibs


# Remove MTB retrieved lib and dependencies
mpy_mtb_deinit:
	$(info )
	$(info Removing mtb_shared and libs folder ...)
	-$(Q) rm -rf boards/$(BOARD)/libs
	-$(Q) rm -rf boards/mtb_shared


# build MTB project
mpy_mtb_build:
	$(info )
	$(info Building $(BOARD) in $(CONFIG) mode using MTB ...)
	$(Q) cd $(BOARD_DIR); $(MAKE) CONFIG=$(MPY_MTB_CONFIG) build


mpy_mtb_clean:
	$(info )
	$(info Removing $(MPY_MTB_BOARD_BUILD_DIR) ...)
	-$(Q) rm -rf $(MPY_MTB_BOARD_BUILD_DIR)


# get variables set for includes, objects, etc. and add to mpy makefile variables
mpy_define_mtb_vars: mpy_mtb_build
#                                  -I. -Ibsps/TARGET_APP_CY8CPROTO-062-4343W -Ibsps -Ibsps/TARGET_APP_CY8CPROTO-062-4343W/bluetooth -Ibsps/TARGET_APP_CY8CPROTO-062-4343W/config/GeneratedSource -Ibsps/TARGET_APP_CY8CPROTO-062-4343W/config          -Ilibs/lwip/src/include/compat/posix -Ilibs/lwip/src/include/compat -Ilibs/lwip/src/include -Ilibs/lwip/src -Ilibs/lwip -Ilibs/lwip/src/include/compat/posix/arpa -Ilibs/lwip/src/include/compat/posix/net -Ilibs/lwip/src/include/compat/posix/sys -Ilibs/lwip/src/include/compat/stdc -Ilibs/lwip/src/include/lwip -Ilibs/lwip/src/include/lwip/apps -Ilibs/lwip/src/include/lwip/priv -Ilibs/lwip/src/include/lwip/prot -Ilibs/lwip/src/include/netif -Ilibs/lwip/src/include/netif/ppp -Ilibs/lwip/src/include/netif/ppp/polarssl -Ilibs/whd-bsp-integration -Ilibs/wifi-connection-manager/include -Ilibs/wifi-connection-manager -Ilibs/wifi-connection-manager/source/COMPONENT_4343W -Ilibs/wifi-connection-manager/source -Ilibs/wifi-connection-manager/source/COMPONENT_MBEDTLS -Ilibs/wifi-host-driver/WiFi_Host_Driver/inc -Ilibs/wifi-host-driver/WiFi_Host_Driver -Ilibs/wifi-host-driver -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/clm/COMPONENT_4343W -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/clm -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/firmware/COMPONENT_4343W -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/firmware -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/nvram/COMPONENT_4343W/COMPONENT_MURATA-1DX -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/nvram/COMPONENT_4343W -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/nvram -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/resource_imp -Ilibs/wifi-host-driver/WiFi_Host_Driver/src/bus_protocols -Ilibs/wifi-host-driver/WiFi_Host_Driver/src -Ilibs/wifi-host-driver/WiFi_Host_Driver/src/include -I../mtb_shared/retarget-io/release-v1.4.0 -I../mtb_shared/serial-flash/release-v1.3.0 -I../mtb_shared/wifi-core-freertos-lwip-mbedtls/release-v1.0.0/configs -I../mtb_shared/wifi-core-freertos-lwip-mbedtls/release-v1.0.0 -I../mtb_shared/abstraction-rtos/release-v1.7.1/include -I../mtb_shared/abstraction-rtos/release-v1.7.1 -I../mtb_shared/abstraction-rtos/release-v1.7.1/include/COMPONENT_FREERTOS -I../mtb_shared/cat1cm0p/release-v1.0.0/COMPONENT_CAT1A -I../mtb_shared/cat1cm0p/release-v1.0.0 -I../mtb_shared/clib-support/release-v1.3.0 -I../mtb_shared/clib-support/release-v1.3.0/TOOLCHAIN_GCC_ARM -I../mtb_shared/cmsis/release-v5.8.0/Core/Include -I../mtb_shared/cmsis/release-v5.8.0/Core -I../mtb_shared/cmsis/release-v5.8.0 -I../mtb_shared/connectivity-utilities/release-v4.1.0 -I../mtb_shared/connectivity-utilities/release-v4.1.0/cy_log -I../mtb_shared/connectivity-utilities/release-v4.1.0/cy_string -I../mtb_shared/connectivity-utilities/release-v4.1.0/JSON_parser -I../mtb_shared/connectivity-utilities/release-v4.1.0/linked_list -I../mtb_shared/connectivity-utilities/release-v4.1.0/network -I../mtb_shared/core-lib/release-v1.3.1/include -I../mtb_shared/core-lib/release-v1.3.1 -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1 -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1/include -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1 -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1/mbedtls_MXCRYPTO -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1/mbedtls_MXCRYPTOLITE -I../mtb_shared/freertos/release-v10.4.305/Source/include -I../mtb_shared/freertos/release-v10.4.305/Source -I../mtb_shared/freertos/release-v10.4.305 -I../mtb_shared/freertos/release-v10.4.305/Source/portable/COMPONENT_CM4 -I../mtb_shared/freertos/release-v10.4.305/Source/portable -I../mtb_shared/freertos/release-v10.4.305/Source/portable/COMPONENT_CM4/TOOLCHAIN_GCC_ARM -I../mtb_shared/lwip-freertos-integration/release-v1.0.0/arch -I../mtb_shared/lwip-freertos-integration/release-v1.0.0 -I../mtb_shared/lwip-network-interface-integration/release-v1.1.1/include -I../mtb_shared/lwip-network-interface-integration/release-v1.1.1 -I../mtb_shared/lwip-network-interface-integration/release-v1.1.1/source -I../mtb_shared/mbedtls/mbedtls-2.25.0/include/mbedtls -I../mtb_shared/mbedtls/mbedtls-2.25.0/include -I../mtb_shared/mbedtls/mbedtls-2.25.0 -I../mtb_shared/mbedtls/mbedtls-2.25.0/include/psa -I../mtb_shared/mbedtls/mbedtls-2.25.0/library -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/COMPONENT_CAT1A/include/pin_packages -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/COMPONENT_CAT1A/include -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/COMPONENT_CAT1A -I../mtb_shared/mtb-hal-cat1/release-v2.1.0 -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/COMPONENT_CAT1A/include/triggers -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/include -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/include_pvt -I../mtb_shared/mtb-hal-cat1/release-v2.1.0/source -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0/devices/COMPONENT_CAT1A/include -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0/devices/COMPONENT_CAT1A -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0/devices -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0 -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0/devices/COMPONENT_CAT1A/include/ip -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0/drivers/include -I../mtb_shared/mtb-pdl-cat1/release-v3.0.0/drivers -I../mtb_shared/secure-sockets/release-v3.2.0/include -I../mtb_shared/secure-sockets/release-v3.2.0 -I../mtb_shared/secure-sockets/release-v3.2.0/include/COMPONENT_FREERTOS -I../mtb_shared/secure-sockets/release-v3.2.0/source -I../mtb_shared/wpa3-external-supplicant/release-v1.1.0/include -I../mtb_shared/wpa3-external-supplicant/release-v1.1.0
#	$(eval MPY_MTB_INCLUDE_DIRS = -I. -Ibsps/TARGET_APP_CY8CPROTO-062-4343W -Ibsps -Ibsps/TARGET_APP_CY8CPROTO-062-4343W/bluetooth -Ibsps/TARGET_APP_CY8CPROTO-062-4343W/config/GeneratedSource -Ibsps/TARGET_APP_CY8CPROTO-062-4343W/config -Isource -Ilibs/lwip/src/include/compat/posix -Ilibs/lwip/src/include/compat -Ilibs/lwip/src/include -Ilibs/lwip/src -Ilibs/lwip -Ilibs/lwip/src/include/compat/posix/arpa -Ilibs/lwip/src/include/compat/posix/net -Ilibs/lwip/src/include/compat/posix/sys -Ilibs/lwip/src/include/compat/stdc -Ilibs/lwip/src/include/lwip -Ilibs/lwip/src/include/lwip/apps -Ilibs/lwip/src/include/lwip/priv -Ilibs/lwip/src/include/lwip/prot -Ilibs/lwip/src/include/netif -Ilibs/lwip/src/include/netif/ppp -Ilibs/lwip/src/include/netif/ppp/polarssl -Ilibs/whd-bsp-integration -Ilibs/wifi-connection-manager/include -Ilibs/wifi-connection-manager -Ilibs/wifi-connection-manager/source/COMPONENT_4343W -Ilibs/wifi-connection-manager/source -Ilibs/wifi-connection-manager/source/COMPONENT_MBEDTLS -Ilibs/wifi-host-driver/WiFi_Host_Driver/inc -Ilibs/wifi-host-driver/WiFi_Host_Driver -Ilibs/wifi-host-driver -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/clm/COMPONENT_4343W -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/clm -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/firmware/COMPONENT_4343W -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/firmware -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/nvram/COMPONENT_4343W/COMPONENT_MURATA-1DX -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/nvram/COMPONENT_4343W -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/nvram -Ilibs/wifi-host-driver/WiFi_Host_Driver/resources/resource_imp -Ilibs/wifi-host-driver/WiFi_Host_Driver/src/bus_protocols -Ilibs/wifi-host-driver/WiFi_Host_Driver/src -Ilibs/wifi-host-driver/WiFi_Host_Driver/src/include -I../mtb_shared/retarget-io/release-v1.4.0 -I../mtb_shared/serial-flash/release-v1.3.0 -I../mtb_shared/wifi-core-freertos-lwip-mbedtls/release-v1.0.0/configs -I../mtb_shared/wifi-core-freertos-lwip-mbedtls/release-v1.0.0 -I../mtb_shared/abstraction-rtos/release-v1.7.1/include -I../mtb_shared/abstraction-rtos/release-v1.7.1 -I../mtb_shared/abstraction-rtos/release-v1.7.1/include/COMPONENT_FREERTOS -I../mtb_shared/cat1cm0p/release-v1.0.0/COMPONENT_CAT1A -I../mtb_shared/cat1cm0p/release-v1.0.0 -I../mtb_shared/clib-support/release-v1.3.0 -I../mtb_shared/clib-support/release-v1.3.0/TOOLCHAIN_GCC_ARM -I../mtb_shared/cmsis/release-v5.8.0/Core/Include -I../mtb_shared/cmsis/release-v5.8.0/Core -I../mtb_shared/cmsis/release-v5.8.0 -I../mtb_shared/connectivity-utilities/release-v4.1.0 -I../mtb_shared/connectivity-utilities/release-v4.1.0/cy_log -I../mtb_shared/connectivity-utilities/release-v4.1.0/cy_string -I../mtb_shared/connectivity-utilities/release-v4.1.0/JSON_parser -I../mtb_shared/connectivity-utilities/release-v4.1.0/linked_list -I../mtb_shared/connectivity-utilities/release-v4.1.0/network -I../mtb_shared/core-lib/release-v1.3.1/include -I../mtb_shared/core-lib/release-v1.3.1 -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1 -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1/include -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1 -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1/mbedtls_MXCRYPTO -I../mtb_shared/cy-mbedtls-acceleration/release-v1.4.1/COMPONENT_CAT1/mbedtls_MXCRYPTOLITE -I../mtb_shared/freertos/release-v10.4.305/Source/include -I../mtb_shared/freertos/release-v10.4.305/Source -I../mtb_shared/freertos/release-v10.4.305 -I../mtb_shared/freertos/release-v10.4.305/Source/portable/COMPONENT_CM4 -I../mtb_shared/freertos/release-v10.4.305/Source/portable -I../mtb_shared/freertos/release-v10.4.305/Source/portable/COMPONENT_CM4/TOOLCHAIN_GCC_ARM -I../mtb_shared/lwip-freertos-integration/release-v1.0.0/arch -I../mtb_shared/lwip-freertos-integration/release-v1.0.0 -I../mtb_shared/lwip-network-interface-integration/release-v1.1.1/include -I../mtb_shared/lwip-network-interface-integration/release-v1.1.1 -I../mtb_shared/lwip-network-interface-integration/release-v1.1.1/source -I../mtb_shared/mbedtls/mbedtls-2.25.0/include/mbedtls -I../mtb_shared/mbedtls/mbedtls-2.25.0/include -I../mtb_shared/mbedtls/mbedtls-2.25.0 -I../mtb_shared/mbedtls/mbedtls-2.25.0/include/psa -I../mtb_shared/mbedtls/mbedtls-2.25.0/library -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/COMPONENT_CAT1A/include/pin_packages -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/COMPONENT_CAT1A/include -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/COMPONENT_CAT1A -I../mtb_shared/mtb-hal-cat1/release-v2.2.0 -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/COMPONENT_CAT1A/include/triggers -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/include -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/include_pvt -I../mtb_shared/mtb-hal-cat1/release-v2.2.0/source -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0/devices/COMPONENT_CAT1A/include -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0/devices/COMPONENT_CAT1A -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0/devices -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0 -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0/devices/COMPONENT_CAT1A/include/ip -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0/drivers/include -I../mtb_shared/mtb-pdl-cat1/release-v3.2.0/drivers -I../mtb_shared/secure-sockets/release-v3.2.0/include -I../mtb_shared/secure-sockets/release-v3.2.0 -I../mtb_shared/secure-sockets/release-v3.2.0/include/COMPONENT_FREERTOS -I../mtb_shared/secure-sockets/release-v3.2.0/source -I../mtb_shared/wpa3-external-supplicant/release-v1.1.0/include -I../mtb_shared/wpa3-external-supplicant/release-v1.1.0)
	@:
	$(eval MPY_MTB_INCLUDE_DIRS = $(file < $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/inclist.rsp))
	$(eval INC += $(subst -I,-I$(BOARD_DIR)/,$(MPY_MTB_INCLUDE_DIRS)))
#	$(info INC : $(INC))
	$(eval MPY_MTB_LIBRARIES = $(file < $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/liblist.rsp))
#	$(info MPY_MTB_LIBRARIES : $(MPY_MTB_LIBRARIES))
	$(eval LIBS += $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/$(MPY_MTB_LIB_NAME))
#	$(eval LIBS += $(MPY_MTB_LIBRARIES) $(MPY_MTB_BOARD_BUILD_OUTPUT_DIR)/$(MPY_MTB_LIB_NAME))
#	$(info LIBS : $(LIBS))
	$(eval CFLAGS +=  -mcpu=cortex-m4 --specs=nano.specs -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb -ffunction-sections -fdata-sections -ffat-lto-objects -g -Wall -pipe -DCOMPONENT_4343W -DCOMPONENT_APP_CY8CPROTO_062_4343W -DCOMPONENT_CAT1 -DCOMPONENT_CAT1A -DCOMPONENT_CM0P_SLEEP -DCOMPONENT_CM4 -DCOMPONENT_CM4_0 -DCOMPONENT_Debug -DCOMPONENT_GCC_ARM -DCOMPONENT_HCI_UART -DCOMPONENT_MURATA_1DX -DCOMPONENT_MW_ABSTRACTION_RTOS -DCOMPONENT_MW_CAT1CM0P -DCOMPONENT_MW_CLIB_SUPPORT -DCOMPONENT_MW_CMSIS -DCOMPONENT_MW_CONNECTIVITY_UTILITIES -DCOMPONENT_MW_CORE_LIB -DCOMPONENT_MW_CORE_MAKE -DCOMPONENT_MW_CY_MBEDTLS_ACCELERATION -DCOMPONENT_MW_FREERTOS -DCOMPONENT_MW_LWIP -DCOMPONENT_MW_LWIP_FREERTOS_INTEGRATION -DCOMPONENT_MW_LWIP_NETWORK_INTERFACE_INTEGRATION -DCOMPONENT_MW_MBEDTLS -DCOMPONENT_MW_MTB_HAL_CAT1 -DCOMPONENT_MW_MTB_PDL_CAT1 -DCOMPONENT_MW_RECIPE_MAKE_CAT1A -DCOMPONENT_MW_RETARGET_IO -DCOMPONENT_MW_SECURE_SOCKETS -DCOMPONENT_MW_SERIAL_FLASH -DCOMPONENT_MW_WHD_BSP_INTEGRATION -DCOMPONENT_MW_WIFI_CONNECTION_MANAGER -DCOMPONENT_MW_WIFI_CORE_FREERTOS_LWIP_MBEDTLS -DCOMPONENT_MW_WIFI_HOST_DRIVER -DCOMPONENT_MW_WPA3_EXTERNAL_SUPPLICANT -DCOMPONENT_PSOC6_02 -DCOMPONENT_SOFTFP -DCOMPONENT_WIFI_INTERFACE_SDIO -DCORE_NAME_CM4_0=1 -DCY8C624ABZI_S2D44 -DCYBSP_WIFI_CAPABLE -DCY_APPNAME_mtb_example_wifi_scan -DCY_RETARGET_IO_CONVERT_LF_TO_CRLF -DCY_RTOS_AWARE -DCY_SUPPORTS_DEVICE_VALIDATION -DCY_TARGET_BOARD=APP_CY8CPROTO_062_4343W -DCY_USING_HAL -DCY_WIFI_HOST_WAKE_SW_FORCE=0 -DDEBUG -DMBEDTLS_USER_CONFIG_FILE=mbedtls_user_config.h -DTARGET_APP_CY8CPROTO_062_4343W  -DCOMPONENT_FREERTOS  -DCOMPONENT_LWIP -DCOMPONENT_MBEDTLS)
#	$(info CFLAGS : $(CFLAGS))
	$(eval CXXFLAGS += $(CFLAGS))
#	$(info CXXFLAGS : $(CXXFLAGS))
	$(eval LDFLAGS += -mcpu=cortex-m4 --specs=nano.specs -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb -ffunction-sections -fdata-sections -ffat-lto-objects -g -Wall -pipe -Wl,--gc-sections -T$(MPY_DIR_OF_MTB_ADAPTER_MAKEFILE)/bsps/TARGET_APP_CY8CPROTO-062-4343W/COMPONENT_CM4/TOOLCHAIN_GCC_ARM/linker.ld -Wl,-Map,$(BUILD)/firmware.map -Wl,--start-group -Wl,--end-group -Wl,--print-memory-usage)
#	$(info LDFLAGS : $(LDFLAGS))
	$(eval QSTR_GEN_CFLAGS += $(INC) $(CFLAGS))


# the following can be used without usbip
mpy_program_win: $(MPY_MAIN_BUILD_DIR)/firmware.hex
	$(info )
	$(info Programming using openocd ...)
	$(OPENOCD_WIN_HOME)/bin/openocd.exe -s $(OPENOCD_WIN_HOME)/scripts -s bsps/TARGET_APP_CY8CPROTO-062-4343W/config/GeneratedSource -c "source [find interface/kitprog3.cfg]; ; source [find target/psoc6_2m.cfg]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program $(MPY_DIR_OF_MAIN_MAKEFILE)/build/firmware.hex verify reset exit;"
	$(info Programming done.)


# WSL : device needs to be attached using usbip
mpy_program_unix: $(MPY_MAIN_BUILD_DIR)/firmware.hex
	$(info )
	$(info Programming using openocd ...)
	$(OPENOCD_HOME)/bin/openocd -s $(OPENOCD_HOME)/scripts -s bsps/TARGET_APP_CY8CPROTO-062-4343W/config/GeneratedSource -c "source [find interface/kitprog3.cfg]; ; source [find target/psoc6_2m.cfg]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program $(MPY_DIR_OF_MAIN_MAKEFILE)/build/firmware.hex verify reset exit;"
	$(info Programming done.)

get_attached_dev:
	$(info )
	$(List of attached devices)
	$(eval attached_dev = $(shell ~/ModusToolbox/tools_3.0/fw-loader/bin/fw-loader --device-list | sed -n -e 's/: KitProg3 CMSIS-DAP BULK-//' -e 's/FW Version[^0-9]*\(\([0-9]\.\)\{0,4\}[0-9][^.]\).*//p'| sed -n 's/^[ \t]*//p'))
	$(info attached devices : $(attached_dev))
	$(foreach device, $(attached_dev), $(info this device is: $(device)))



mpy_program: mpy_program_unix


.PHONY: mpy_mtb_init mpy_mtb_deinit mpy_mtb_build mpy_define_mtb_vars mpy_program get_attached_dev