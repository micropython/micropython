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

CROSS_COMPILE = xtensa-esp32s2-elf-

#######################################
# CFLAGS
#######################################

INC += -I.
INC += -I./boards
INC += -I./boards/$(BOARD)
INC += -I./peripherals
INC += -I../..
INC += -I../../lib/mp-readline
INC += -I../../lib/tinyusb/src
INC += -I../../supervisor/shared/usb
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I$(BUILD)/esp-idf/config

INC += -isystem esp-idf
INC += -isystem esp-idf/components/app_update/include
INC += -isystem esp-idf/components/bootloader_support/include
INC += -isystem esp-idf/components/driver/esp32s2/include
INC += -isystem esp-idf/components/driver/include
INC += -isystem esp-idf/components/esp32s2/include
INC += -isystem esp-idf/components/esp_common/include
INC += -isystem esp-idf/components/esp_event/include
INC += -isystem esp-idf/components/esp_hw_support/include
INC += -isystem esp-idf/components/esp_netif/include
INC += -isystem esp-idf/components/esp_pm/include
INC += -isystem esp-idf/components/esp_ringbuf/include
INC += -isystem esp-idf/components/esp_rom/include
INC += -isystem esp-idf/components/esp_system/include
INC += -isystem esp-idf/components/esp_timer/include
INC += -isystem esp-idf/components/esp_wifi/include
INC += -isystem esp-idf/components/freertos/include
INC += -isystem esp-idf/components/freertos/include/freertos
INC += -isystem esp-idf/components/freertos/port/xtensa/include
INC += -isystem esp-idf/components/hal/include
INC += -isystem esp-idf/components/hal/esp32s2/include
INC += -isystem esp-idf/components/heap/include
INC += -isystem esp-idf/components/log/include/
INC += -isystem esp-idf/components/lwip/lwip/src/include
INC += -isystem esp-idf/components/lwip/port/esp32/include
INC += -isystem esp-idf/components/lwip/include/apps/sntp
INC += -isystem esp-idf/components/mbedtls/mbedtls/include
INC += -isystem esp-idf/components/mbedtls/port/include/
INC += -isystem esp-idf/components/newlib/platform_include
INC += -isystem esp-idf/components/nvs_flash/include
INC += -isystem esp-idf/components/soc/include
INC += -isystem esp-idf/components/soc/esp32s2/include
INC += -isystem esp-idf/components/spi_flash/include
INC += -isystem esp-idf/components/xtensa/esp32s2/include
INC += -isystem esp-idf/components/xtensa/include

CFLAGS += -DHAVE_CONFIG_H \
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

CFLAGS += $(INC) -Werror -Wall -mlongcalls -std=gnu11 -Wl,--gc-sections $(BASE_CFLAGS) $(C_DEFS) $(CFLAGS_MOD) $(COPT)

LDFLAGS = $(CFLAGS) -Wl,-nostdlib -Wl,-Map=$@.map -Wl,-cref -Wl,--undefined=uxTopUsedPriority

LDFLAGS += -L$(BUILD)/esp-idf/esp-idf/esp32s2 \
		   -L$(BUILD)/esp-idf/esp-idf/esp32s2/ld \
		   -Lesp-idf/components/esp32s2/ld \
		   -Lesp-idf/components/esp_rom/esp32s2/ld \
		   -Tesp32s2_out.ld \
		   -Tesp32s2.project.ld \
		   -Tesp32s2.peripherals.ld \
		   -Tesp32s2.rom.ld \
		   -Tesp32s2.rom.api.ld \
		   -Tesp32s2.rom.libgcc.ld \
		   -Tesp32s2.rom.newlib-data.ld \
		   -Tesp32s2.rom.newlib-funcs.ld \
		   -Tesp32s2.rom.spiflash.ld

LDFLAGS += -Wl,-Bstatic \
		   -Wl,--no-warn-mismatch \
		   -Wl,--build-id=none \
		   -fno-rtti

LIBS := -lgcc -lc -lstdc++

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32S2 -DCFG_TUSB_OS=OPT_OS_FREERTOS -DCFG_TUD_CDC_RX_BUFSIZE=1024 -DCFG_TUD_CDC_TX_BUFSIZE=1024
CFLAGS += -DCFG_TUD_MSC_BUFSIZE=4096 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_MIDI_TX_BUFSIZE=128
CFLAGS += -DCFG_TUD_VENDOR_RX_BUFSIZE=128 -DCFG_TUD_VENDOR_TX_BUFSIZE=128


######################################
# source
######################################


SRC_C += \
	background.c \
	cam.c \
	fatfs_port.c \
	mphalport.c \
	bindings/espidf/__init__.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	modules/$(CIRCUITPY_MODULE).c \
	lib/netutils/netutils.c \
	peripherals/timer.c \
	peripherals/touch.c \
	peripherals/pcnt.c \
	peripherals/pins.c \
	peripherals/rmt.c

ifneq ($(CIRCUITPY_USB),0)
SRC_C += lib/tinyusb/src/portable/espressif/esp32sx/dcd_esp32sx.c
endif

SRC_S =

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
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
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

$(BUILD)/lib/protomatter/src/core.o: CFLAGS += -DESP32

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR +=

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
	IDF_PATH=$(IDF_PATH) cmake -S . -B $(BUILD)/esp-idf -DSDKCONFIG=$(BUILD)/esp-idf/sdkconfig -DSDKCONFIG_DEFAULTS="$(SDKCONFIGS)" -DCMAKE_TOOLCHAIN_FILE=$(IDF_PATH)/tools/cmake/toolchain-esp32s2.cmake -DIDF_TARGET=esp32s2 -GNinja

# build a lib
# Adding -d explain -j 1 -v to the ninja line will output debug info
#$(BUILD)/esp-idf/esp-idf/%.a: $(BUILD)/esp-idf/config/sdkconfig.h
#	ninja -C $(BUILD)/esp-idf esp-idf/$*.a

$(BUILD)/esp-idf/esp-idf/esp32s2/esp32s2_out.ld: $(BUILD)/esp-idf/config/sdkconfig.h
	ninja -C $(BUILD)/esp-idf esp-idf/esp32s2/esp32s2_out.ld

$(BUILD)/esp-idf/esp-idf/esp32s2/ld/esp32s2.project.ld: $(BUILD)/esp-idf/config/sdkconfig.h
	ninja -C $(BUILD)/esp-idf esp-idf/esp32s2/ld/esp32s2.project.ld


$(BUILD)/esp-idf/partition_table/partition-table.bin: $(BUILD)/esp-idf/config/sdkconfig.h
	IDF_PATH=$(IDF_PATH) ninja -C $(BUILD)/esp-idf partition_table/partition-table.bin

# run menuconfig and then remove standard settings
menuconfig: $(BUILD)/esp-idf/config
	$(Q)ninja -C $(BUILD)/esp-idf menuconfig
	$(Q)diff --old-line-format= --unchanged-line-format= esp-idf-config/sdkconfig.defaults $(BUILD)/esp-idf/sdkconfig > $(BUILD)/sdkconfig.diff || true
	$(Q)grep -Fvxf $(DEBUG_SDKCONFIG) -f $(FLASH_SDKCONFIG) $(BUILD)/sdkconfig.diff > boards/$(BOARD)/sdkconfig

# qstr builds include headers so we need to make sure they are up to date
$(HEADER_BUILD)/qstr.split: | $(BUILD)/esp-idf/config/sdkconfig.h

ESP_IDF_COMPONENTS_LINK = app_update bootloader_support driver efuse esp32s2 esp_adc_cal esp_common esp_event esp_hw_support esp_netif esp_pm esp_ringbuf esp_rom esp_system esp_timer esp-tls esp_wifi freertos hal heap log lwip mbedtls newlib nvs_flash pthread soc spi_flash vfs wpa_supplicant xtensa

ESP_IDF_COMPONENTS_EXPANDED = $(foreach component, $(ESP_IDF_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/$(component)/lib$(component).a)
ESP_IDF_WIFI_COMPONENTS_EXPANDED = $(foreach component, $(ESP_IDF_WIFI_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/$(component)/lib$(component).a)

MBEDTLS_COMPONENTS_LINK = crypto tls x509
MBEDTLS_COMPONENTS_LINK_EXPANDED = $(foreach component, $(MBEDTLS_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/library/libmbed$(component).a)

BINARY_BLOBS = esp-idf/components/xtensa/esp32s2/libxt_hal.a
BINARY_WIFI_BLOBS = libcoexist.a libcore.a libespnow.a libmesh.a libnet80211.a libpp.a librtc.a libsmartconfig.a libphy.a
BINARY_BLOBS += $(addprefix esp-idf/components/esp_wifi/lib/esp32s2/, $(BINARY_WIFI_BLOBS))

ESP_IDF_COMPONENTS_EXPANDED += esp-idf/components/xtensa/esp32s2/libxt_hal.a
ESP_AUTOGEN_LD = $(BUILD)/esp-idf/esp-idf/esp32s2/esp32s2_out.ld $(BUILD)/esp-idf/esp-idf/esp32s2/ld/esp32s2.project.ld

FLASH_FLAGS = --flash_mode $(CIRCUITPY_ESP_FLASH_MODE) --flash_freq $(CIRCUITPY_ESP_FLASH_FREQ) --flash_size $(CIRCUITPY_ESP_FLASH_SIZE)

ESPTOOL_FLAGS ?= -b 460800 --before=default_reset --after=no_reset

all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

.PHONY: esp-idf-stamp
esp-idf-stamp: $(BUILD)/esp-idf/config/sdkconfig.h
	$(Q)ninja -C $(BUILD)/esp-idf \
		bootloader/bootloader.bin \
		esp-idf/app_update/libapp_update.a \
		esp-idf/bootloader_support/libbootloader_support.a \
		esp-idf/driver/libdriver.a \
		esp-idf/efuse/libefuse.a \
		esp-idf/esp32s2/libesp32s2.a \
		esp-idf/esp32s2/ld/esp32s2.project.ld \
		esp-idf/esp_adc_cal/libesp_adc_cal.a \
		esp-idf/esp_common/libesp_common.a \
		esp-idf/esp_event/libesp_event.a \
		esp-idf/esp_hw_support/libesp_hw_support.a \
		esp-idf/esp_netif/libesp_netif.a \
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
		esp-idf/xtensa/libxtensa.a

$(BUILD)/firmware.elf: $(OBJ) | esp-idf-stamp
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(ESP_IDF_COMPONENTS_EXPANDED) $(BINARY_BLOBS) $(MBEDTLS_COMPONENTS_LINK_EXPANDED) $(BUILD)/esp-idf/esp-idf/newlib/libnewlib.a  -Wl,--end-group -u newlib_include_pthread_impl -Wl,--start-group $(LIBS) -Wl,--end-group $(BUILD)/esp-idf/esp-idf/pthread/libpthread.a  -u __cxx_fatal_exception

$(BUILD)/circuitpython-firmware.bin: $(BUILD)/firmware.elf | tools/build_memory_info.py
	$(STEPECHO) "Create $@"
	$(Q)esptool.py --chip esp32s2 elf2image $(FLASH_FLAGS) --elf-sha256-offset 0xb0 -o $@ $^
	$(Q)$(PYTHON3) tools/build_memory_info.py $< $(BUILD)/esp-idf/sdkconfig $@

$(BUILD)/firmware.bin: $(BUILD)/circuitpython-firmware.bin | esp-idf-stamp
	$(Q)$(PYTHON) ../../tools/join_bins.py $@ 0x1000 $(BUILD)/esp-idf/bootloader/bootloader.bin 0x8000 $(BUILD)/esp-idf/partition_table/partition-table.bin 0x10000 $(BUILD)/circuitpython-firmware.bin

$(BUILD)/firmware.uf2: $(BUILD)/circuitpython-firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON3) $(TOP)/tools/uf2/utils/uf2conv.py -f 0xbfdd4eee -b 0x0000 -c -o $@ $^

flash: $(BUILD)/firmware.bin
	esptool.py --chip esp32s2 -p $(PORT) $(ESPTOOL_FLAGS) write_flash $(FLASH_FLAGS) 0x0000 $^

flash-circuitpython-only: $(BUILD)/circuitpython-firmware.bin
	esptool.py --chip esp32s2 -p $(PORT) $(ESPTOOL_FLAGS) write_flash $(FLASH_FLAGS) 0x10000 $^

monitor: $(BUILD)/firmware.elf
	cp $< build/circuitpython.elf
	idf.py monitor -p $(PORT)

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
