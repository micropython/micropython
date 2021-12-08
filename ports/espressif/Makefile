# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Select the board to build for.
ifeq ($(BOARD),)
  $(error You must provide a BOARD parameter)
else
  ifeq ($(wildcard boards/$(BOARD)/.),)
    $(error Invalid BOARD specified)
  endif
endif

# If the flash PORT is not given, use the default /dev/tty.SLAB_USBtoUART.
PORT ?= /dev/tty.SLAB_USBtoUART

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk

# Board-specific
include boards/$(BOARD)/mpconfigboard.mk

# Port-specific
include mpconfigport.mk

# CircuitPython-specific
include $(TOP)/py/circuitpy_mpconfig.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

# Include make rules and variables common across CircuitPython builds.
include $(TOP)/py/circuitpy_defns.mk

ifeq ($(IDF_TARGET),esp32c3)
IDF_TARGET_ARCH = riscv
CROSS_COMPILE = riscv32-esp-elf-
else
IDF_TARGET_ARCH = xtensa
CROSS_COMPILE = xtensa-$(IDF_TARGET)-elf-
endif

#######################################
# CFLAGS
#######################################

INC += \
	-I.\
	-I./boards \
	-I./boards/$(BOARD) \
	-I./peripherals \
	-I../.. \
	-I../../lib/mp-readline \
	-I../../lib/tinyusb/src \
	-I../../supervisor/shared/usb \
	-I$(BUILD) \
	-I$(BUILD)/genhdr \
	-I$(BUILD)/esp-idf/config \
	-isystem esp-idf \
	-isystem esp-idf/components/app_update/include \
	-isystem esp-idf/components/bootloader_support/include \
	-isystem esp-idf/components/driver/include \
	-isystem esp-idf/components/driver/$(IDF_TARGET)/include \
	-isystem esp-idf/components/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_adc_cal/include \
	-isystem esp-idf/components/esp_common/include \
	-isystem esp-idf/components/esp_event/include \
	-isystem esp-idf/components/esp_hw_support/include \
	-isystem esp-idf/components/esp_hw_support/include/soc \
	-isystem esp-idf/components/esp_netif/include \
	-isystem esp-idf/components/esp_pm/include \
	-isystem esp-idf/components/esp_ringbuf/include \
	-isystem esp-idf/components/esp_rom/include \
	-isystem esp-idf/components/esp_system/include \
	-isystem esp-idf/components/esp_timer/include \
	-isystem esp-idf/components/esp_wifi/include \
	-isystem esp-idf/components/freertos/include \
	-isystem esp-idf/components/freertos/include/freertos \
	-isystem esp-idf/components/freertos/include/esp_additions \
	-isystem esp-idf/components/freertos/include/esp_additions/freertos \
	-isystem esp-idf/components/freertos/port/$(IDF_TARGET_ARCH)/include \
	-isystem esp-idf/components/hal/include \
	-isystem esp-idf/components/hal/$(IDF_TARGET)/include \
	-isystem esp-idf/components/hal/platform_port/include \
	-isystem esp-idf/components/heap/include \
	-isystem esp-idf/components/log/include \
	-isystem esp-idf/components/lwip/include \
	-isystem esp-idf/components/lwip/lwip/src/include \
	-isystem esp-idf/components/lwip/port/esp32/include \
	-isystem esp-idf/components/mbedtls/esp_crt_bundle/include \
	-isystem esp-idf/components/mbedtls/mbedtls/include \
	-isystem esp-idf/components/mbedtls/port/include \
	-isystem esp-idf/components/newlib/platform_include \
	-isystem esp-idf/components/nvs_flash/include \
	-isystem esp-idf/components/soc/include \
	-isystem esp-idf/components/soc/$(IDF_TARGET)/include \
	-isystem esp-idf/components/spi_flash/include \
	-isystem esp-idf/components/$(IDF_TARGET_ARCH)/include \
	-isystem esp-idf/components/$(IDF_TARGET_ARCH)/$(IDF_TARGET)/include

# See https://github.com/espressif/esp-idf/issues/6906
ifeq ($(IDF_TARGET),esp32c3)
CFLAGS += -include "esp32c3_fix.h"
endif

CFLAGS += \
	-DHAVE_CONFIG_H \
	-DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\" \
	-DUNITY_INCLUDE_CONFIG_H -DWITH_POSIX

# Make our canary value match FreeRTOS's
# This define is in FreeRTOS as tskSTACK_FILL_BYTE 0xa5U which we expand out to a full word.
CFLAGS += -DSTACK_CANARY_VALUE=0xa5a5a5a5

#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -DDEBUG -ggdb
  OPTIMIZATION_FLAGS ?= -Og
  # You may want to enable these flags to make setting breakpoints easier.
  # CFLAGS += -fno-inline -fno-ipa-sra
else
  CFLAGS += -DNDEBUG -ggdb3
  OPTIMIZATION_FLAGS ?= -O2
  # TODO: Test with -flto
  ### CFLAGS += -flto
endif

# option to override compiler optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

CFLAGS += $(INC) -Werror -Wall -std=gnu11 -Wl,--gc-sections $(BASE_CFLAGS) $(C_DEFS) $(CFLAGS_MOD) $(COPT) -Werror=missing-prototypes

ifeq ($(IDF_TARGET_ARCH),xtensa)
CFLAGS += -mlongcalls
endif

LDFLAGS = $(CFLAGS) -Wl,-nostdlib -Wl,-Map=$@.map -Wl,-cref -Wl,--undefined=uxTopUsedPriority

LDFLAGS += \
	-L$(BUILD)/esp-idf/esp-idf/esp_system/ld \
	-Lesp-idf/components/esp_rom/$(IDF_TARGET)/ld \
	-Lesp-idf/components/soc/$(IDF_TARGET)/ld \
	-Tmemory.ld \
	-Tsections.ld \
	-T$(IDF_TARGET).peripherals.ld \
	-T$(IDF_TARGET).rom.ld \
	-T$(IDF_TARGET).rom.api.ld \
	-T$(IDF_TARGET).rom.libgcc.ld \
	-T$(IDF_TARGET).rom.newlib-nano.ld \
	-Wl,-Bstatic \
	-Wl,--no-warn-mismatch \
	-Wl,--build-id=none \
	-fno-rtti

ifeq ($(IDF_TARGET),esp32c3)
LDFLAGS += \
	-Tesp32c3.rom.newlib.ld \
	-Tesp32c3.rom.version.ld \
	-Tesp32c3.rom.eco3.ld
else ifeq ($(IDF_TARGET),esp32s2)
LDFLAGS += \
	-T$(IDF_TARGET).rom.newlib-data.ld \
	-T$(IDF_TARGET).rom.newlib-funcs.ld \
	-T$(IDF_TARGET).rom.spiflash.ld
else ifeq ($(IDF_TARGET),esp32s3)
LDFLAGS += \
	-Tesp32s3.rom.newlib.ld \
	-Tesp32s3.rom.version.ld
endif

LIBS := -lgcc -lc -lstdc++

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# TinyUSB defines
ifeq ($(CIRCUITPY_USB),1)
CFLAGS += \
	-DCFG_TUSB_MCU=OPT_MCU_ESP32S2 \
	-DCFG_TUSB_OS=OPT_OS_FREERTOS \
	-DCFG_TUD_CDC_RX_BUFSIZE=1024 \
	-DCFG_TUD_CDC_TX_BUFSIZE=1024 \
	-DCFG_TUD_MSC_BUFSIZE=4096 \
	-DCFG_TUD_MIDI_RX_BUFSIZE=128 \
	-DCFG_TUD_MIDI_TX_BUFSIZE=128 \
	-DCFG_TUD_VENDOR_RX_BUFSIZE=128 \
	-DCFG_TUD_VENDOR_TX_BUFSIZE=128
endif

######################################
# source
######################################

SRC_C += \
	background.c \
	fatfs_port.c \
	mphalport.c \
	bindings/espidf/__init__.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	modules/$(CIRCUITPY_MODULE).c \
	shared/netutils/netutils.c \
	peripherals/i2c.c \
	peripherals/rmt.c \
	peripherals/timer.c \
	peripherals/$(IDF_TARGET)/pins.c

ifneq ($(IDF_TARGET),esp32c3)
SRC_C += \
	peripherals/pcnt.c \
	peripherals/touch.c
ifeq ($(IDF_TARGET),esp32s2)
SRC_C += \
	cam.c \
	i2s_lcd_esp32s2_driver.c
endif
endif

$(BUILD)/i2s_lcd_esp32s2_driver.o: CFLAGS += -Wno-sign-compare

ifneq ($(CIRCUITPY_USB),0)
SRC_C += lib/tinyusb/src/portable/espressif/esp32sx/dcd_esp32sx.c
endif

SRC_COMMON_HAL_EXPANDED = \
	$(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
	$(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
	$(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = \
	$(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
	$(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
	$(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

ifneq ($(FROZEN_MPY_DIR),)
FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))
endif

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

$(BUILD)/lib/protomatter/src/core.o: CFLAGS += -DESP32

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)

# IDF build commands
IDF_PATH = $(realpath ./esp-idf)
# create the directory
$(BUILD)/esp-idf:
	$(Q)$(MKDIR) -p $@

FLASH_SDKCONFIG = esp-idf-config/sdkconfig-$(CIRCUITPY_ESP_FLASH_SIZE).defaults
ifeq ($(DEBUG), 1)
	DEBUG_SDKCONFIG = esp-idf-config/sdkconfig-debug.defaults
else
	DEBUG_SDKCONFIG = esp-idf-config/sdkconfig-opt.defaults
endif
SDKCONFIGS = esp-idf-config/sdkconfig.defaults;$(DEBUG_SDKCONFIG);$(FLASH_SDKCONFIG);boards/$(BOARD)/sdkconfig

# create the config headers
$(BUILD)/esp-idf/config/sdkconfig.h: boards/$(BOARD)/sdkconfig | $(BUILD)/esp-idf
	IDF_PATH=$(IDF_PATH) cmake -S . -B $(BUILD)/esp-idf -DSDKCONFIG=$(BUILD)/esp-idf/sdkconfig -DSDKCONFIG_DEFAULTS="$(SDKCONFIGS)" -DCMAKE_TOOLCHAIN_FILE=$(IDF_PATH)/tools/cmake/toolchain-$(IDF_TARGET).cmake -DIDF_TARGET=$(IDF_TARGET) -GNinja

# build a lib
# Adding -d explain -j 1 -v to the ninja line will output debug info
#$(BUILD)/esp-idf/esp-idf/%.a: $(BUILD)/esp-idf/config/sdkconfig.h
#	ninja -C $(BUILD)/esp-idf esp-idf/$*.a

$(BUILD)/esp-idf/esp-idf/$(IDF_TARGET)/$(IDF_TARGET)_out.ld: $(BUILD)/esp-idf/config/sdkconfig.h
	ninja -C $(BUILD)/esp-idf esp-idf/$(IDF_TARGET)/$(IDF_TARGET)_out.ld

$(BUILD)/esp-idf/esp-idf/$(IDF_TARGET)/ld/$(IDF_TARGET).project.ld: $(BUILD)/esp-idf/config/sdkconfig.h
	ninja -C $(BUILD)/esp-idf esp-idf/$(IDF_TARGET)/ld/$(IDF_TARGET).project.ld

$(BUILD)/esp-idf/partition_table/partition-table.bin: $(BUILD)/esp-idf/config/sdkconfig.h
	IDF_PATH=$(IDF_PATH) ninja -C $(BUILD)/esp-idf partition_table/partition-table.bin

# run menuconfig and then remove standard settings
menuconfig: $(BUILD)/esp-idf/config
	$(Q)ninja -C $(BUILD)/esp-idf menuconfig
	$(Q)diff --old-line-format= --unchanged-line-format= esp-idf-config/sdkconfig.defaults $(BUILD)/esp-idf/sdkconfig > $(BUILD)/sdkconfig.diff || true
	$(Q)grep -Fvxf $(DEBUG_SDKCONFIG) -f $(FLASH_SDKCONFIG) $(BUILD)/sdkconfig.diff > boards/$(BOARD)/sdkconfig

# qstr builds include headers so we need to make sure they are up to date
$(HEADER_BUILD)/qstr.split: | $(BUILD)/esp-idf/config/sdkconfig.h

BINARY_WIFI_BLOBS = libcoexist.a libcore.a libespnow.a libmesh.a libnet80211.a libpp.a libsmartconfig.a libwapi.a
BINARY_BLOBS = esp-idf/components/esp_phy/lib/$(IDF_TARGET)/libphy.a $(addprefix esp-idf/components/esp_wifi/lib/$(IDF_TARGET)/, $(BINARY_WIFI_BLOBS))

ESP_IDF_COMPONENTS_LINK = $(IDF_TARGET_ARCH) app_update bootloader_support driver efuse esp_adc_cal esp_common esp_event esp_hw_support esp_ipc esp_netif esp_pm esp_phy esp_ringbuf esp_rom esp_system esp_timer esp-tls esp_wifi freertos hal heap log lwip mbedtls newlib nvs_flash pthread soc spi_flash vfs wpa_supplicant
ESP_IDF_COMPONENTS_EXPANDED = $(foreach component, $(ESP_IDF_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/$(component)/lib$(component).a)

MBEDTLS_COMPONENTS_LINK = crypto tls x509
MBEDTLS_COMPONENTS_LINK_EXPANDED = $(foreach component, $(MBEDTLS_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/library/libmbed$(component).a)

ifeq ($(IDF_TARGET_ARCH),xtensa)
BINARY_BLOBS += esp-idf/components/xtensa/$(IDF_TARGET)/libxt_hal.a
ESP_IDF_COMPONENTS_EXPANDED += esp-idf/components/xtensa/$(IDF_TARGET)/libxt_hal.a
endif

ifeq ($(IDF_TARGET),esp32c3)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32s3)
BOOTLOADER_OFFSET = 0x0
else
BOOTLOADER_OFFSET = 0x1000
endif

PARTITION_TABLE_OFFSET = 0x8000
FIRMWARE_OFFSET = 0x10000

ESP_AUTOGEN_LD = $(BUILD)/esp-idf/esp-idf/$(IDF_TARGET)/$(IDF_TARGET)_out.ld $(BUILD)/esp-idf/esp-idf/$(IDF_TARGET)/ld/$(IDF_TARGET).project.ld

FLASH_FLAGS = --flash_mode $(CIRCUITPY_ESP_FLASH_MODE) --flash_freq $(CIRCUITPY_ESP_FLASH_FREQ) --flash_size $(CIRCUITPY_ESP_FLASH_SIZE)

ESPTOOL_FLAGS ?= --before=default_reset --after=no_reset

ifeq ($(IDF_TARGET),esp32c3)
all: $(BUILD)/firmware.bin
else
all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2
endif

.PHONY: esp-idf-stamp
esp-idf-stamp: $(BUILD)/esp-idf/config/sdkconfig.h
	$(Q)ninja -C $(BUILD)/esp-idf \
		bootloader/bootloader.bin \
		esp-idf/esp_system/__ldgen_output_sections.ld \
		esp-idf/app_update/libapp_update.a \
		esp-idf/bootloader_support/libbootloader_support.a \
		esp-idf/driver/libdriver.a \
		esp-idf/efuse/libefuse.a \
		esp-idf/esp_adc_cal/libesp_adc_cal.a \
		esp-idf/esp_common/libesp_common.a \
		esp-idf/esp_event/libesp_event.a \
		esp-idf/esp_hw_support/libesp_hw_support.a \
		esp-idf/esp_ipc/libesp_ipc.a \
		esp-idf/esp_netif/libesp_netif.a \
		esp-idf/esp_phy/libesp_phy.a \
		esp-idf/esp_pm/libesp_pm.a \
		esp-idf/esp_ringbuf/libesp_ringbuf.a \
		esp-idf/esp_rom/libesp_rom.a \
		esp-idf/esp_system/libesp_system.a \
		esp-idf/esp_timer/libesp_timer.a \
		esp-idf/esp-tls/libesp-tls.a \
		esp-idf/esp_wifi/libesp_wifi.a \
		esp-idf/freertos/libfreertos.a \
		esp-idf/hal/libhal.a \
		esp-idf/heap/libheap.a \
		esp-idf/log/liblog.a \
		esp-idf/lwip/liblwip.a \
		esp-idf/mbedtls/libmbedtls.a \
		esp-idf/newlib/libnewlib.a \
		esp-idf/nvs_flash/libnvs_flash.a \
		esp-idf/pthread/libpthread.a \
		esp-idf/soc/libsoc.a \
		esp-idf/spi_flash/libspi_flash.a \
		esp-idf/vfs/libvfs.a \
		esp-idf/wpa_supplicant/libwpa_supplicant.a \
		esp-idf/$(IDF_TARGET_ARCH)/lib$(IDF_TARGET_ARCH).a

$(BUILD)/firmware.elf: $(OBJ) | esp-idf-stamp
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(ESP_IDF_COMPONENTS_EXPANDED) $(BINARY_BLOBS) $(MBEDTLS_COMPONENTS_LINK_EXPANDED) $(BUILD)/esp-idf/esp-idf/newlib/libnewlib.a  -Wl,--end-group -u newlib_include_pthread_impl -Wl,--start-group $(LIBS) -Wl,--end-group $(BUILD)/esp-idf/esp-idf/pthread/libpthread.a  -u __cxx_fatal_exception

$(BUILD)/circuitpython-firmware.bin: $(BUILD)/firmware.elf | tools/build_memory_info.py
	$(STEPECHO) "Create $@"
	$(Q)esptool.py --chip $(IDF_TARGET) elf2image $(FLASH_FLAGS) --elf-sha256-offset 0xb0 -o $@ $^
	$(Q)$(PYTHON) tools/build_memory_info.py $< $(BUILD)/esp-idf/sdkconfig $@

$(BUILD)/firmware.bin: $(BUILD)/circuitpython-firmware.bin | esp-idf-stamp
	$(Q)$(PYTHON) ../../tools/join_bins.py $@ $(BOOTLOADER_OFFSET) $(BUILD)/esp-idf/bootloader/bootloader.bin $(PARTITION_TABLE_OFFSET) $(BUILD)/esp-idf/partition_table/partition-table.bin $(FIRMWARE_OFFSET) $(BUILD)/circuitpython-firmware.bin

$(BUILD)/firmware.uf2: $(BUILD)/circuitpython-firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -f 0xbfdd4eee -b 0x0000 -c -o $@ $^

flash: $(BUILD)/firmware.bin
	esptool.py --chip $(IDF_TARGET) -p $(PORT) $(ESPTOOL_FLAGS) write_flash $(FLASH_FLAGS) 0x0000 $^

flash-circuitpython-only: $(BUILD)/circuitpython-firmware.bin
	esptool.py --chip $(IDF_TARGET) -p $(PORT) $(ESPTOOL_FLAGS) write_flash $(FLASH_FLAGS) $(FIRMWARE_OFFSET) $^

monitor: $(BUILD)/firmware.elf
	cp $< build/circuitpython.elf
	idf.py monitor -p $(PORT)

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
