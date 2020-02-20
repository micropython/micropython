# Select the board to build for: if not given on the command line,
# then default to GENERIC.
BOARD ?= GENERIC

PORT_DIR := $(MPY_DIR)/ports/esp32

# If the build directory is not given, make it reflect the board name.
BUILD ?= build

BOARD_DIR ?= $(PORT_DIR)/boards/$(BOARD)
BOARD_BUILD ?= $(PORT_DIR)/build-$(BOARD)
#ifeq ($(wildcard $(BOARD_DIR)/.),)
#$(error Invalid BOARD specified: $(BOARD_DIR))
#endif
#
#include ../../py/mkenv.mk
#
## Optional (not currently used for ESP32)
#-include mpconfigport.mk
#
#ifneq ($(SDKCONFIG),)
#$(error Use the BOARD variable instead of SDKCONFIG)
#endif

# Expected to set SDKCONFIG
include $(BOARD_DIR)/mpconfigboard.mk
SDKCONFIG := $(addprefix $(PORT_DIR)/,$(SDKCONFIG))

## qstr definitions (must come before including py.mk)
#QSTR_DEFS = qstrdefsport.h
#QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h
#QSTR_GLOBAL_REQUIREMENTS = $(SDKCONFIG_H)

#MICROPY_PY_USSL = 0
#MICROPY_SSL_AXTLS = 0
#MICROPY_PY_BTREE = 1
#MICROPY_VFS_FAT = 1
#MICROPY_VFS_LFS2 = 1

#FROZEN_MANIFEST ?= boards/manifest.py

## include py core make definitions
#include $(TOP)/py/py.mk

#GIT_SUBMODULES = lib/berkeley-db-1.xx

#PORT ?= /dev/ttyUSB0
#BAUD ?= 460800
#FLASH_MODE ?= dio
#FLASH_FREQ ?= 40m
#FLASH_SIZE ?= 4MB
#CROSS_COMPILE ?= xtensa-esp32-elf-
#OBJDUMP = $(CROSS_COMPILE)objdump

#SDKCONFIG_COMBINED = $(BUILD)/sdkconfig.combined
#SDKCONFIG_H = $(BOARD_BUILD)/sdkconfig.h

# the git hash of the currently supported ESP IDF version
ESPIDF_SUPHASH_V3 := 6ccb4cf5b7d1fdddb8c2492f9cbc926abaf230df
ESPIDF_SUPHASH_V4 := 463a9d8b7f9af8205222b80707f9bdbba7c530e1

define print_supported_git_hash
$(info Supported git hash (v3.3): $(ESPIDF_SUPHASH_V3))
$(info Supported git hash (v4.0) (experimental): $(ESPIDF_SUPHASH_V4))
endef

# paths to ESP IDF and its components
ifeq ($(ESPIDF),)
ifneq ($(IDF_PATH),)
ESPIDF = $(IDF_PATH)
else
$(info The ESPIDF variable has not been set, please set it to the root of the esp-idf repository.)
$(info See README.md for installation instructions.)
$(call print_supported_git_hash)
$(error ESPIDF not set)
endif
endif

ESPCOMP = $(ESPIDF)/components
#ESPTOOL ?= $(ESPCOMP)/esptool_py/esptool/esptool.py
ESPCOMP_KCONFIGS = $(shell find $(ESPCOMP) -name Kconfig)
ESPCOMP_KCONFIGS_PROJBUILD = $(shell find $(ESPCOMP) -name Kconfig.projbuild)

# verify the ESP IDF version
ESPIDF_CURHASH := $(shell git -C $(ESPIDF) show -s --pretty=format:'%H')

ifeq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH_V3))
$(info Building with ESP IDF v3)
else ifeq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH_V4))
$(info Building with ESP IDF v4)

PYPARSING_VERSION = $(shell python3 -c 'import pyparsing; print(pyparsing.__version__)')
ifneq ($(PYPARSING_VERSION),2.3.1)
$(info ** ERROR **)
$(info EDP IDF requires pyparsing version less than 2.4)
$(info You will need to set up a Python virtual environment with pyparsing 2.3.1)
$(info Please see README.md for more information)
$(error Incorrect pyparsing version)
endif
else
$(info ** WARNING **)
$(info The git hash of ESP IDF does not match the supported version)
$(info The build may complete and the firmware may work but it is not guaranteed)
$(info ESP IDF path:       $(ESPIDF))
$(info Current git hash:   $(ESPIDF_CURHASH))
$(call print_supported_git_hash)
endif

# pretty format of ESP IDF version, used internally by the IDF
IDF_VER := $(shell git -C $(ESPIDF) describe)

#ifeq ($(shell which $(CC) 2> /dev/null),)
#$(info ** ERROR **)
#$(info Cannot find C compiler $(CC))
#$(info Add the xtensa toolchain to your PATH. See README.md)
#$(error C compiler missing)
#endif

## Support BLE by default when building with IDF 4.x.
## Can be explicitly disabled on the command line or board config.
#ifeq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH_V4))
#MICROPY_PY_BLUETOOTH ?= 1
#ifeq ($(MICROPY_PY_BLUETOOTH),1)
#SDKCONFIG += boards/sdkconfig.ble
#MICROPY_BLUETOOTH_NIMBLE = 1
#endif
#endif
#
# include sdkconfig to get needed configuration values
include $(SDKCONFIG)
$(shell cp $(BOARD_BUILD)/sdkconfig* $(BUILD))

################################################################################
# Compiler and linker flags

INC_ESPCOMP += -I$(PORT_DIR)
#INC += -I$(TOP)
#INC += -I$(TOP)/lib/mp-readline
#INC += -I$(TOP)/lib/netutils
#INC += -I$(TOP)/lib/timeutils
INC_ESPCOMP += -I$(BUILD)

INC_ESPCOMP += -I$(ESPCOMP)/bootloader_support/include
INC_ESPCOMP += -I$(ESPCOMP)/bootloader_support/include_bootloader
INC_ESPCOMP += -I$(ESPCOMP)/console
INC_ESPCOMP += -I$(ESPCOMP)/driver/include
INC_ESPCOMP += -I$(ESPCOMP)/driver/include/driver
INC_ESPCOMP += -I$(ESPCOMP)/efuse/include
INC_ESPCOMP += -I$(ESPCOMP)/efuse/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/espcoredump/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/heap/include
INC_ESPCOMP += -I$(ESPCOMP)/log/include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/platform_include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/include
INC_ESPCOMP += -I$(ESPCOMP)/nvs_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/freertos/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_ringbuf/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_event/include
INC_ESPCOMP += -I$(ESPCOMP)/tcpip_adapter/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/lwip/src/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/port/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/apps
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/mbedtls/include
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/port/include
INC_ESPCOMP += -I$(ESPCOMP)/mdns/include
INC_ESPCOMP += -I$(ESPCOMP)/mdns/private_include
INC_ESPCOMP += -I$(ESPCOMP)/spi_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/ulp/include
INC_ESPCOMP += -I$(ESPCOMP)/vfs/include
INC_ESPCOMP += -I$(ESPCOMP)/xtensa-debug-module/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/port/include
INC_ESPCOMP += -I$(ESPCOMP)/app_trace/include
INC_ESPCOMP += -I$(ESPCOMP)/app_update/include
INC_ESPCOMP += -I$(ESPCOMP)/pthread/include
INC_ESPCOMP += -I$(ESPCOMP)/smartconfig_ack/include
INC_ESPCOMP += -I$(ESPCOMP)/sdmmc/include

ifeq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH_V4))
INC_ESPCOMP += -I$(ESPCOMP)/esp_common/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_eth/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_event/private_include
INC_ESPCOMP += -I$(ESPCOMP)/esp_rom/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_wifi/include
INC_ESPCOMP += -I$(ESPCOMP)/esp_wifi/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/apps/sntp
INC_ESPCOMP += -I$(ESPCOMP)/spi_flash/private_include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/include/esp_supplicant
INC_ESPCOMP += -I$(ESPCOMP)/xtensa/include
INC_ESPCOMP += -I$(ESPCOMP)/xtensa/esp32/include
ifeq ($(CONFIG_BT_NIMBLE_ENABLED),y)
INC_ESPCOMP += -I$(ESPCOMP)/bt/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/common/osi/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/common/btc/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/common/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/porting/nimble/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/port/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/ans/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/bas/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/gap/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/gatt/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/ias/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/lls/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/services/tps/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/util/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/store/ram/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/nimble/host/store/config/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/porting/npl/freertos/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/nimble/ext/tinycrypt/include
INC_ESPCOMP += -I$(ESPCOMP)/bt/host/nimble/esp-hci/include
endif
else
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/expat/expat/expat/lib
INC_ESPCOMP += -I$(ESPCOMP)/expat/port/include
INC_ESPCOMP += -I$(ESPCOMP)/json/include
INC_ESPCOMP += -I$(ESPCOMP)/json/port/include
INC_ESPCOMP += -I$(ESPCOMP)/micro-ecc/micro-ecc
INC_ESPCOMP += -I$(ESPCOMP)/nghttp/port/include
INC_ESPCOMP += -I$(ESPCOMP)/nghttp/nghttp2/lib/includes
endif

ifeq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH_V4))
ifeq ($(MICROPY_PY_BLUETOOTH),1)
CFLAGS_MOD += -DMICROPY_PY_BLUETOOTH=1
CFLAGS_MOD += -DMICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE=1

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
CFLAGS_MOD += -DMICROPY_BLUETOOTH_NIMBLE=1
endif
endif
endif

CFLAGS = $(INC_ESPCOMP) $(CFLAGS_MOD)
