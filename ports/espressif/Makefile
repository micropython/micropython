# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
#
# SPDX-License-Identifier: MIT

include ../../py/circuitpy_mkenv.mk

# Keep dependency selection identical during automatic CMake reconfiguration.
export IDF_COMPONENT_MANAGER = 0

ifeq ($(IDF_TARGET),esp32s3)
BT_IDF_TARGET = esp32c3
else ifeq ($(IDF_TARGET),esp32c61)
BT_IDF_TARGET = esp32c6
else
BT_IDF_TARGET = $(IDF_TARGET)
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
	-isystem esp-idf/components/bootloader_support/bootloader_flash/include \
	-isystem esp-idf/components/bt/include/$(BT_IDF_TARGET)/include \
	-isystem esp-idf/components/bt/host/nimble/esp-hci/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/controller/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/host/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/host/services/ans/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/host/services/gap/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/host/services/gatt/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/host/util/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/nimble/transport/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/porting/nimble/include \
	-isystem esp-idf/components/bt/host/nimble/nimble/porting/npl/freertos/include \
	-isystem esp-idf/components/bt/host/nimble/port/include \
	-isystem esp-idf/components/bt/porting/include \
	-isystem esp-idf/components/bt/porting/npl/freertos/include \
	-isystem esp-idf/components/bt/porting/transport/include \
	-isystem esp-idf/components/esp_driver_touch_sens/include \
	-isystem esp-idf/components/driver/twai/include \
	-isystem esp-idf/components/driver/i2c/include \
	-isystem esp-idf/components/efuse/include \
	-isystem esp-idf/components/efuse/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_adc/include \
	-isystem esp-idf/components/esp_adc/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_app_format/include \
	-isystem esp-idf/components/esp_bootloader_format/include \
	-isystem esp-idf/components/esp_common/include \
	-isystem esp-idf/components/esp_driver_dac/include \
	-isystem esp-idf/components/esp_driver_gpio/include \
	-isystem esp-idf/components/esp_driver_gptimer/include \
	-isystem esp-idf/components/esp_driver_i2c/include \
	-isystem esp-idf/components/esp_driver_i2s/include \
	-isystem esp-idf/components/esp_driver_jpeg/include \
	-isystem esp-idf/components/esp_driver_ledc/include \
	-isystem esp-idf/components/esp_driver_parlio/include \
	-isystem esp-idf/components/esp_driver_pcnt/include \
	-isystem esp-idf/components/esp_driver_rmt/include \
	-isystem esp-idf/components/esp_driver_sdio/include \
	-isystem esp-idf/components/esp_driver_sdmmc/include \
	-isystem esp-idf/components/esp_driver_sdmmc/legacy/include \
	-isystem esp-idf/components/esp_driver_dma/include \
	-isystem esp-idf/components/esp_driver_spi/include \
	-isystem esp-idf/components/esp_driver_tsens/include \
	-isystem esp-idf/components/esp_driver_uart/include \
	-isystem esp-idf/components/esp_event/include \
	-isystem esp-idf/components/esp_hw_support/include \
	-isystem esp-idf/components/esp_hw_support/include/soc \
	-isystem esp-idf/components/esp_hw_support/port/$(IDF_TARGET)/private_include \
	-isystem esp-idf/components/esp_hw_support/etm/include \
	-isystem esp-idf/components/esp_mm/include \
	-isystem esp-idf/components/esp_eth/include \
	-isystem esp-idf/components/esp_netif/include \
	-isystem esp-idf/components/esp_blockdev/include \
	-isystem esp-idf/components/esp_partition/include \
	-isystem esp-idf/components/esp_pm/include \
	-isystem esp-idf/components/esp_psram/include \
	-isystem esp-idf/components/esp_ringbuf/include \
	-isystem esp-idf/components/esp_rom/include \
	-isystem esp-idf/components/esp_rom/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_rom/$(IDF_TARGET)/include/$(IDF_TARGET) \
	-isystem esp-idf/components/esp_security/include \
	-isystem esp-idf/components/esp_system/include \
	-isystem esp-idf/components/esp_timer/include \
	-isystem esp-idf/components/esp_wifi/include \
	-isystem esp-idf/components/esp_wifi/include/local \
	-isystem esp-idf/components/freertos/config/include \
	-isystem esp-idf/components/freertos/config/include/freertos \
	-isystem esp-idf/components/freertos/config/$(IDF_TARGET_ARCH)/include \
	-isystem esp-idf/components/freertos/esp_additions/include \
	-isystem esp-idf/components/freertos/esp_additions/include/freertos \
	-isystem esp-idf/components/freertos/FreeRTOS-Kernel/include \
	-isystem esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos \
	-isystem esp-idf/components/freertos/FreeRTOS-Kernel/portable/$(IDF_TARGET_ARCH)/include \
	-isystem esp-idf/components/freertos/FreeRTOS-Kernel/portable/$(IDF_TARGET_ARCH)/include/freertos \
	-isystem esp-idf/components/hal/include \
	-isystem esp-idf/components/hal/$(IDF_TARGET)/include \
	-isystem esp-idf/components/hal/platform_port/include \
	-isystem esp-idf/components/esp_hal_gpio/include \
	-isystem esp-idf/components/esp_hal_gpio/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_timg/include \
	-isystem esp-idf/components/esp_hal_ana_conv/include \
	-isystem esp-idf/components/esp_hal_touch_sens/include \
	-isystem esp-idf/components/esp_hal_uart/include \
	-isystem esp-idf/components/esp_hal_gpspi/include \
	-isystem esp-idf/components/esp_hal_i2c/include \
	-isystem esp-idf/components/esp_hal_i2s/include \
	-isystem esp-idf/components/esp_hal_i2s/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_jpeg/include \
	-isystem esp-idf/components/esp_hal_jpeg/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_ledc/include \
	-isystem esp-idf/components/esp_hal_ledc/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_pcnt/include \
	-isystem esp-idf/components/esp_hal_rmt/include \
	-isystem esp-idf/components/esp_hal_rmt/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_twai/include \
	-isystem esp-idf/components/esp_hal_twai/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_timg/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_gpspi/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_dma/include \
	-isystem esp-idf/components/esp_hal_dma/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_emac/include \
	-isystem esp-idf/components/esp_hal_emac/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_lcd/include \
	-isystem esp-idf/components/esp_hal_lcd/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_sd/include \
	-isystem esp-idf/components/esp_hal_sd/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_usb/include \
	-isystem esp-idf/components/esp_hal_usb/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_parlio/include \
	-isystem esp-idf/components/esp_hal_pmu/include \
	-isystem esp-idf/components/esp_hal_pmu/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_wdt/include \
	-isystem esp-idf/components/esp_hal_wdt/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_security/include \
	-isystem esp-idf/components/esp_hal_regi2c/include \
	-isystem esp-idf/components/esp_hal_regi2c/$(IDF_TARGET)/include \
	-isystem esp-idf/components/esp_hal_systimer/include \
	-isystem esp-idf/components/heap/include \
	-isystem esp-idf/components/log/include \
	-isystem esp-idf/components/lwip/include \
	-isystem esp-idf/components/lwip/lwip/src/include \
	-isystem esp-idf/components/lwip/port/include \
	-isystem esp-idf/components/lwip/port/esp32xx/include \
	-isystem esp-idf/components/lwip/port/freertos/include \
	-isystem esp-idf/components/mbedtls/mbedtls/include \
	-isystem esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/include \
	-isystem esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/include \
	-isystem esp-idf/components/mbedtls/port/include \
	-isystem esp-idf/components/mbedtls/port/psa_driver/include \
	-isystem esp-idf/components/esp_libc/platform_include \
	-isystem esp-idf/components/nvs_flash/include \
	-isystem esp-idf/components/sdmmc/include \
	-isystem esp-idf/components/soc/include \
	-isystem esp-idf/components/soc/$(IDF_TARGET)/include \
	-isystem esp-idf/components/soc/$(IDF_TARGET)/register \
	-isystem esp-idf/components/spi_flash/include \
	-isystem esp-idf/components/ulp/ulp_fsm/include \
	-isystem esp-idf/components/ulp/ulp_riscv/include \
	-isystem esp-idf/components/ulp/ulp_common/include \
	-isystem esp-idf/components/$(IDF_TARGET_ARCH)/include \
	-isystem esp-idf/components/$(IDF_TARGET_ARCH)/$(IDF_TARGET)/include \
	-isystem esp-protocols/components/mdns/include

CFLAGS += \
	-DHAVE_CONFIG_H \
	-DESP_PLATFORM=1 \
	-DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\" \
	-DMBEDTLS_DECLARE_PRIVATE_IDENTIFIERS \
	-DMBEDTLS_PADLOCK_FILE=\"ports/espressif/esp-idf/components/mbedtls/mbedtls/library/padlock.h\" \
	-DUNITY_INCLUDE_CONFIG_H -DWITH_POSIX \
	-DMP3DEC_GENERIC \
	-DSOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE \
	-DSOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ

# Make our canary value match FreeRTOS's
# This define is in FreeRTOS as tskSTACK_FILL_BYTE 0xa5U which we expand out to a full word.
CFLAGS += -DSTACK_CANARY_VALUE=0xa5a5a5a5

# IDF 5.3 uses a new ESP_SYSTEM_INIT_FN macro to "register" functions to run on
# init. They work by placing function pointers into a linker section that ends
# up as a function pointer array. To ensure the linker includes these functions,
# one must provide `-u` arguments to state the symbols are missing. This would
# normally happen implicitly by another function calling to these.
REGISTRATION_FUNCTIONS = \
	-u ld_include_highint_hdl \
	-u __cxx_fatal_exception \
	-u __cxx_init_dummy \
	-u __cxa_guard_dummy \
	-u esp_app_desc \
	-u esp_timer_init_include_func \
	-u uart_vfs_include_dev_init \
	-u esp_vfs_include_console_register \
	-u __ubsan_include \
	-u esp_system_include_startup_funcs \
	-u esp_efuse_startup_include_func \
	-u esp_libc_include_heap_impl \
	-u esp_libc_include_reent_syscalls_impl \
	-u esp_libc_include_syscalls_impl \
	-u esp_libc_include_pthread_impl \
	-u esp_libc_include_assert_impl \
	-u esp_libc_include_getentropy_impl \
	-u esp_libc_include_init_funcs \
	-u esp_libc_init_funcs \
	-u include_esp_phy_override \
	-u vfs_include_syscalls_impl \
	-u esp_vfs_include_nullfs_register \
	-u usb_serial_jtag_vfs_include_dev_init \
	-u usb_serial_jtag_connection_monitor_include \
	-u esp_flash_spi_init_include_func \
	-u pthread_include_pthread_impl \
	-u pthread_include_pthread_cond_var_impl \
	-u pthread_include_pthread_local_storage_impl \
	-u pthread_include_pthread_rwlock_impl \
	-u pthread_include_pthread_semaphore_impl \
	-u esp_security_init_include_impl \
	-u mbedtls_psa_crypto_init_include_impl

# Video DMA destinations come from the Python heap (user bytearrays), and
# the cache line is 64 bytes on the P4, so make GC blocks 64 bytes as well.
# supervisor/port.c aligns the PSRAM heap areas to the same value.
ifeq ($(IDF_TARGET),esp32p4)
CFLAGS += -DMICROPY_BYTES_PER_GC_BLOCK=64
endif

# RISC-V picolibc ships optimized memcpy/memset/etc. that need forcing
# (gated by CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS in IDF's esp_libc CMake).
ifeq ($(IDF_TARGET_ARCH),riscv)
REGISTRATION_FUNCTIONS += \
	-u esp_libc_include_memcpy_impl \
	-u esp_libc_include_memmove_impl \
	-u esp_libc_include_memcmp_impl \
	-u esp_libc_include_memset_impl \
	-u esp_libc_include_strcpy_impl \
	-u esp_libc_include_strncpy_impl \
	-u esp_libc_include_strcmp_impl \
	-u esp_libc_include_strncmp_impl
endif

# Chips with PAU + TOP power-down (SOC_PAU_SUPPORTED && SOC_PM_SUPPORT_TOP_PD):
# C5, C6, C61, H2, H21, H4, P4. These pull in sleep_gpio + system-peripheral
# retention, and the SW-driven RISC-V CPU register save/restore path.
ifneq ($(filter $(IDF_TARGET),esp32c5 esp32c6 esp32c61 esp32h2 esp32h21 esp32h4 esp32p4 esp32s31),)
REGISTRATION_FUNCTIONS += \
	-u esp_sleep_gpio_include \
	-u sleep_system_peripheral_dummy \
	-u rv_core_critical_regs_save \
	-u rv_core_critical_regs_restore
endif

# RISC-V chips with a hardware FPU (H4, P4, S31) need FPU register save/restore
# alongside the CPU retention path.
ifneq ($(filter $(IDF_TARGET),esp32h4 esp32p4 esp32s31),)
REGISTRATION_FUNCTIONS += \
	-u rv_core_fpu_save \
	-u rv_core_fpu_restore
endif

ifneq ($(CIRCUITPY_WIFI),0)
ifeq ($(filter $(IDF_TARGET),esp32s2 esp32p4),)
ifneq ($(CIRCUITPY_ESP_HOSTED),1)
REGISTRATION_FUNCTIONS += -u esp_coex_init_include_func
endif
endif
endif


#Debugging/Optimization
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb
	ifeq ($(IDF_TARGET_ARCH),riscv)
		OPTIMIZATION_FLAGS ?= -Os
		CFLAGS += -DNDEBUG
	else
		OPTIMIZATION_FLAGS ?= -Og
		CFLAGS += -DDEBUG
	endif
#	You may want to enable these flags to make setting breakpoints easier.
#	CFLAGS += -fno-inline -fno-ipa-sra
else
	CFLAGS += -DNDEBUG
#	RISC-V is larger than xtensa
#	Use -Os for RISC-V when it overflows
	ifeq ($(IDF_TARGET_ARCH),riscv)
		OPTIMIZATION_FLAGS ?= -Os
	else
		OPTIMIZATION_FLAGS ?= -O2
	endif
endif

# option to override compiler optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

# Default ROM symbol maps; per-chip blocks below may override.
IDF_TARGET_ROM_LD ?= $(IDF_TARGET).rom.ld
# Floating-point/libgcc ROM map. IDF picks between libgcc and rvfp based on
# CONFIG_COMPILER_FLOAT_LIB_FROM_GCCLIB (rvfp xor libgcc, never both). Default
# to libgcc; targets that ship the rvfp variant in ROM (P4 in particular)
# override this.
IDF_TARGET_ROM_FLOAT_LD ?= $(IDF_TARGET).rom.libgcc.ld

CFLAGS += $(INC) -Werror -Wall -std=gnu11 -Wl,--gc-sections $(BASE_CFLAGS) $(C_DEFS) $(CFLAGS_MOD) $(COPT) -Werror=missing-prototypes -Werror=old-style-definition -Wno-error=cpp -Wno-cpp

# ESP-IDF v6.0 uses picolibc instead of newlib.
CFLAGS += --specs=picolibc.specs

ifeq ($(IDF_TARGET_ARCH),xtensa)
# Remove the last two flags once TinyUSB is updated with the `#include <xtensa_api.h>` instead of
# `#include "xtensa/xtensa_api.h"`.

	CFLAGS += -mlongcalls -fno-builtin-memcpy -fno-builtin-memset -fno-builtin-bzero
	CFLAGS += -isystem esp-idf/components/xtensa/deprecated_include/
ifeq ($(IDF_TARGET),esp32)
	CFLAGS += -Wno-frame-address
endif
	CFLAGS += -DMICROPY_GCREGS_SETJMP=1

# Wrap longjmp with a patched version that protects register window update with a critical section
	LDFLAGS += -Wl,--wrap=longjmp

	SRC_C += shared/runtime/gchelper_generic.c
else ifeq ($(IDF_TARGET_ARCH),riscv)

	ifneq ($(filter $(IDF_TARGET),esp32p4 esp32s31),)
		CFLAGS += -march=rv32imafc_zicsr_zifencei_zaamo_zalrsc -mabi=ilp32f
	else ifeq ($(IDF_TARGET),$(filter $(IDF_TARGET),esp32c2 esp32c3))
		CFLAGS += -march=rv32imc_zicsr_zifencei
	else
		CFLAGS += -march=rv32imac_zicsr_zifencei_zaamo_zalrsc
	endif

	LDFLAGS += \
		-Lesp-idf/components/riscv/ld \
		-Trom.api.ld

	SRC_C += shared/runtime/gchelper_native.c
	SRC_S = shared/runtime/gchelper_rv32i.s
endif


LDFLAGS += $(CFLAGS) -nostartfiles -Wl,-nostdlib -Wl,-Map=$@.map -Wl,-cref -Wl,--undefined=uxTopUsedPriority

LDFLAGS += \
	-L$(BUILD)/esp-idf/esp-idf/esp_system/ld \
	-Lesp-idf/components/esp_rom/$(IDF_TARGET)/ld \
	-Lesp-idf/components/soc/$(IDF_TARGET)/ld \
	-Lesp-idf/components/esp_hal_wdt/$(IDF_TARGET) \
	-Lesp-idf/components/esp_hal_systimer/$(IDF_TARGET) \
	-Lesp-idf/components/heap/port/$(IDF_TARGET)/ld \
	-Lesp-idf/components/esp_hal_regi2c/$(IDF_TARGET) \
	-Tmemory.ld \
	-Tsections.ld \
	-T$(IDF_TARGET).peripherals.ld \
	-T$(IDF_TARGET_ROM_LD) \
	-T$(IDF_TARGET).rom.api.ld \
	-T$(IDF_TARGET_ROM_FLOAT_LD) \
	-Wl,-Bstatic \
	-Wl,--no-warn-mismatch \
	-Wl,--build-id=none \
	-fno-rtti

ifeq ($(IDF_TARGET),esp32)
LDFLAGS += \
	-Tesp32.rom.libc-funcs.ld

CFLAGS += -isystem esp-idf/components/esp_driver_touch_sens/hw_ver1/include

CHIP_COMPONENTS = \
	esp_driver_dac \
	esp_driver_touch_sens \
	esp_hal_emac \
	esp_hal_i2s \
	esp_hal_lcd \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_touch_sens \
	esp_hal_twai

else ifeq ($(IDF_TARGET),esp32c2)
LDFLAGS += \
	-Tesp32c2.rom.ble.ld \
	-Tesp32c2.rom.heap.ld \
	-Tesp32c2.rom.libc.ld \
	-Tesp32c2.rom.version.ld \
	-Trom.systimer.ld \
	-Trom.wdt.ld

CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_hal_dma \
	esp_hal_pmu

else ifeq ($(IDF_TARGET),esp32c3)
# esp32c2 has none of: esp_hal_i2s esp_hal_lcd esp_hal_pcnt esp_hal_touch_sens esp_hal_twai
LDFLAGS += \
	-Tesp32c3.rom.libc.ld \
	-Tesp32c3.rom.version.ld \
	-Tesp32c3.rom.eco3_bt_funcs.ld \
	-Tesp32c3.rom.eco3.ld \
	-Tesp32c3.rom.bt_funcs.ld

CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32c5)
LDFLAGS += \
	-Tesp32c5.rom.phy.ld \
	-Tesp32c5.rom.pp.ld \
	-Tesp32c5.rom.net80211.ld \
	-Tesp32c5.rom.libc.ld \
	-Tesp32c5.rom.version.ld \
	-Tesp32c5.rom.coexist.ld \
	-Tesp32c5.rom.heap.ld \
	-Trom.systimer.ld \
	-Trom.wdt.ld

CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32c6)
LDFLAGS += \
	-Tesp32c6.rom.phy.ld \
	-Tesp32c6.rom.pp.ld \
	-Tesp32c6.rom.net80211.ld \
	-Tesp32c6.rom.libc.ld \
	-Tesp32c6.rom.coexist.ld \
	-Tesp32c6.rom.heap.ld \
	-Trom.systimer.ld \
	-Trom.wdt.ld


CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32c61)
LDFLAGS += \
	-Tesp32c61.rom.phy.ld \
	-Tesp32c61.rom.pp.ld \
	-Tesp32c61.rom.net80211.ld \
	-Tesp32c61.rom.libc.ld \
	-Tesp32c61.rom.version.ld \
	-Tesp32c61.rom.coexist.ld \
	-Tesp32c61.rom.heap.ld \
	-Trom.systimer.ld \
	-Trom.wdt.ld


CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_pmu \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32p4)
CIRCUITPY_ESP32P4_REV ?= 1
CFLAGS += \
	-isystem esp-idf/components/soc/esp32p4/register/hw_ver$(CIRCUITPY_ESP32P4_REV) \
	-isystem esp-idf/components/esp_hw_support/ldo/include \
	-isystem esp-idf/components/esp_driver_touch_sens/hw_ver3/include

# JPEG engine DMA writes into GC blocks, so make them one 64-byte cache line.
CFLAGS += -DMICROPY_BYTES_PER_GC_BLOCK=64

ifeq ($(CIRCUITPY_ESP32P4_REV),3)
IDF_TARGET_ROM_FLOAT_LD = esp32p4.rom.rvfp.ld
LDFLAGS += \
	-Trom.systimer.ld \
	-Tesp32p4.rom.libc.ld \
	-Tesp32p4.rom.version.ld \
	-Trom.wdt.ld
else
IDF_TARGET_ROM_LD = esp32p4.rom.eco0_4.ld
IDF_TARGET_ROM_FLOAT_LD = esp32p4.rom.eco0_4.rvfp.ld
LDFLAGS += \
	-Trom.systimer.ld \
	-Tesp32p4.rom.eco0_4.libc.ld \
	-Tesp32p4.rom.version.ld \
	-Trom.wdt.ld
endif


CHIP_COMPONENTS = \
	esp_driver_cam \
	esp_driver_isp \
	esp_driver_touch_sens \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_cam \
	esp_hal_dma \
	esp_hal_emac \
	esp_hal_i2s \
	esp_hal_lcd \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_touch_sens \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32s31)
CFLAGS += -isystem esp-idf/components/esp_hw_support/ldo/include

# JPEG engine DMA writes into GC blocks, so make them one 64-byte cache line.
CFLAGS += -DMICROPY_BYTES_PER_GC_BLOCK=64

IDF_TARGET_ROM_FLOAT_LD = esp32s31.rom.rvfp.ld
LDFLAGS += \
	-Tesp32s31.rom.heap.ld \
	-Trom.systimer.ld \
	-Tesp32s31.rom.libc.ld \
	-Tesp32s31.rom.version.ld \
	-Trom.wdt.ld

CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_lcd \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_touch_sens \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32h2)
LDFLAGS += \
	-Tesp32h2.rom.heap.ld \
	-Tesp32h2.rom.libc.ld \
	-Trom.systimer.ld \
	-Trom.wdt.ld

CHIP_COMPONENTS = \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32s2)
CFLAGS += -isystem esp-idf/components/esp_driver_touch_sens/hw_ver2/include

LDFLAGS += \
	-Tesp32s2.rom.libc-funcs.ld \
	-Tesp32s2.rom.spiflash_legacy.ld

CHIP_COMPONENTS = \
	esp_driver_dac \
	esp_driver_touch_sens \
	esp_driver_tsens \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_lcd \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_touch_sens \
	esp_hal_twai \
	esp_hal_usb

else ifeq ($(IDF_TARGET),esp32s3)
CFLAGS += -isystem esp-idf/components/esp_driver_touch_sens/hw_ver2/include

LDFLAGS += \
	-Tesp32s3.rom.libc.ld \
	-Tesp32s3.rom.version.ld \
	-Trom.systimer.ld \
	-Tesp32s3.rom.bt_funcs.ld \
	-Trom.wdt.ld

CHIP_COMPONENTS = \
	esp_driver_touch_sens \
	esp_driver_tsens \
	esp_driver_usb_serial_jtag \
	esp_hal_dma \
	esp_hal_i2s \
	esp_hal_lcd \
	esp_hal_pcnt \
	esp_hal_pmu \
	esp_hal_rmt \
	esp_hal_touch_sens \
	esp_hal_twai \
	esp_hal_usb

endif

LIBS := -lgcc -lc -lstdc++

# Use toolchain libm if we're not using our own.
ifneq ($(INTERNAL_LIBM),1)
LIBS += -lm
endif

# TinyUSB defines
# Always add these because we might be doing host.
ifeq ($(IDF_TARGET),esp32)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32
else ifeq ($(IDF_TARGET),esp32c2)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32C2
else ifeq ($(IDF_TARGET),esp32c3)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32C3
else ifeq ($(IDF_TARGET),esp32c5)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32C5
else ifeq ($(IDF_TARGET),esp32c6)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32C6
else ifeq ($(IDF_TARGET),esp32c61)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32C61
else ifeq ($(IDF_TARGET),esp32p4)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32P4
else ifeq ($(IDF_TARGET),esp32s31)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32S31
else ifeq ($(IDF_TARGET),esp32h2)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32H2
else ifeq ($(IDF_TARGET),esp32s2)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32S2
else ifeq ($(IDF_TARGET),esp32s3)
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_ESP32S3
endif
ifeq ($(CIRCUITPY_TINYUSB),1)
CFLAGS += \
	-DCFG_TUSB_OS=OPT_OS_FREERTOS \
	-DCFG_TUD_TASK_QUEUE_SZ=32
endif
ifeq ($(CIRCUITPY_USB_DEVICE),1)
ifeq ($(IDF_TARGET),esp32s2)
# Make more room in internal RAM on the S2.
CFLAGS += \
	-DCFG_TUD_CDC_RX_BUFSIZE=128 \
	-DCFG_TUD_CDC_TX_BUFSIZE=128 \
	-DCFG_TUD_MSC_BUFSIZE=1024
else
CFLAGS += \
	-DCFG_TUD_CDC_RX_BUFSIZE=1024 \
	-DCFG_TUD_CDC_TX_BUFSIZE=1024 \
	-DCFG_TUD_MSC_BUFSIZE=4096
endif
CFLAGS += \
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
	mphalport.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	shared/netutils/netutils.c \
	peripherals/$(IDF_TARGET)/pins.c

ifeq ($(CIRCUITPY_SSL),1)
# The same root certificate bundle and lookup code as the other ports, not ESP-IDF's.
# A board short of flash can set CIRCUITPY_SSL_ROOTS to roots-filtered.pem.
# The generated bundle holds a hash of each root's subject name, not the name itself;
# the format is described in lib/mbedtls_config/crt_bundle.c.
CIRCUITPY_SSL_ROOTS ?= $(TOP)/lib/certificates/data/roots-full.pem
SRC_C += lib/mbedtls_config/crt_bundle.c
$(BUILD)/x509_crt_bundle.S: $(CIRCUITPY_SSL_ROOTS) $(TOP)/tools/gen_crt_bundle.py
	$(Q)$(PYTHON) $(TOP)/tools/gen_crt_bundle.py -i $< -o $@ --asm
OBJ_CRT_BUNDLE := $(BUILD)/x509_crt_bundle.o
endif

SRC_C += $(wildcard common-hal/espidf/*.c)

ifneq ($(CIRCUITPY_ESP_USB_SERIAL_JTAG),0)
SRC_C += supervisor/usb_serial_jtag.c
endif

ifneq ($(CIRCUITPY_TOUCHIO_USE_NATIVE),0)
SRC_C += peripherals/touch.c
endif

ifneq ($(CIRCUITPY_USB_DEVICE),0)
SRC_C += lib/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c
endif

ifeq ($(CIRCUITPY_USB_HOST),1)
SRC_C += lib/tinyusb/src/portable/synopsys/dwc2/hcd_dwc2.c
endif

ifneq ($(filter 1,$(CIRCUITPY_USB_DEVICE) $(CIRCUITPY_USB_HOST)),)
SRC_C += lib/tinyusb/src/portable/synopsys/dwc2/dwc2_common.c
endif

ifneq ($(CIRCUITPY_AUDIOBUSIO),0)
CHIP_COMPONENTS += esp_driver_i2s
endif

ifneq ($(CIRCUITPY_AUDIOI2SIN),0)
CHIP_COMPONENTS += esp_driver_i2s
endif

ifneq ($(CIRCUITPY_BLEIO_NATIVE),0)
SRC_C += common-hal/_bleio/ble_events.c
endif

ifneq ($(CIRCUITPY_DOTCLOCKFRAMEBUFFER),0)
CFLAGS += \
	-isystem esp-idf/components/esp_lcd/include \
	-isystem esp-idf/components/esp_lcd/interface \
	-isystem esp-idf/components/esp_lcd/rgb/include
endif

ifneq ($(CIRCUITPY_MIPIDSI),0)
CFLAGS += \
	-isystem esp-idf/components/esp_lcd/include \
	-isystem esp-idf/components/esp_lcd/interface \
	-isystem esp-idf/components/esp_lcd/dsi/include
endif

ifneq ($(CIRCUITPY_MIPICSI),0)
CFLAGS += \
	-isystem esp-idf/components/esp_driver_cam/include \
	-isystem esp-idf/components/esp_driver_cam/csi/include \
	-isystem esp-idf/components/esp_driver_isp/include \
	-isystem esp-idf/components/esp_hal_cam/include
endif

ifneq ($(CIRCUITPY_ESPCAMERA),0)
SRC_CAMERA := \
	$(wildcard common-hal/espcamera/*.c) \
	$(wildcard bindings/espcamera/*.c)
SRC_C += $(SRC_CAMERA)
CFLAGS += -isystem esp-camera/driver/include
CFLAGS += -isystem esp-camera/conversions/include
endif

ifneq ($(CIRCUITPY_RCLCPY),0)
CFLAGS += -isystem microros-lib/include
CFLAGS += -isystem microros-lib/include/rcl
CFLAGS += -isystem microros-lib/include/statistics_msgs
CFLAGS += -isystem microros-lib/include/composition_interfaces
CFLAGS += -isystem microros-lib/include/example_interfaces
CFLAGS += -isystem microros-lib/include/rmw_microxrcedds_c
CFLAGS += -isystem microros-lib/include/test_msgs
CFLAGS += -isystem microros-lib/include/std_msgs
CFLAGS += -isystem microros-lib/include/rcutils
CFLAGS += -isystem microros-lib/include/lifecycle_msgs
CFLAGS += -isystem microros-lib/include/rosidl_typesupport_interface
CFLAGS += -isystem microros-lib/include/service_msgs
CFLAGS += -isystem microros-lib/include/visualization_msgs
CFLAGS += -isystem microros-lib/include/rosidl_dynamic_typesupport
CFLAGS += -isystem microros-lib/include/stereo_msgs
CFLAGS += -isystem microros-lib/include/ucdr
CFLAGS += -isystem microros-lib/include/rosidl_typesupport_c
CFLAGS += -isystem microros-lib/include/std_srvs
CFLAGS += -isystem microros-lib/include/rcl_lifecycle
CFLAGS += -isystem microros-lib/include/action_msgs
CFLAGS += -isystem microros-lib/include/micro_ros_utilities
CFLAGS += -isystem microros-lib/include/rcl_action
CFLAGS += -isystem microros-lib/include/rcl_logging_interface
CFLAGS += -isystem microros-lib/include/type_description_interfaces
CFLAGS += -isystem microros-lib/include/nav_msgs
CFLAGS += -isystem microros-lib/include/actionlib_msgs
CFLAGS += -isystem microros-lib/include/rmw
CFLAGS += -isystem microros-lib/include/rclc_parameter
CFLAGS += -isystem microros-lib/include/geometry_msgs
CFLAGS += -isystem microros-lib/include/sensor_msgs
CFLAGS += -isystem microros-lib/include/trajectory_msgs
CFLAGS += -isystem microros-lib/include/shape_msgs
CFLAGS += -isystem microros-lib/include/rosidl_runtime_c
CFLAGS += -isystem microros-lib/include/rclc
CFLAGS += -isystem microros-lib/include/rosgraph_msgs
CFLAGS += -isystem microros-lib/include/rclc_lifecycle
CFLAGS += -isystem microros-lib/include/rcl_interfaces
CFLAGS += -isystem microros-lib/include/diagnostic_msgs
CFLAGS += -isystem microros-lib/include/micro_ros_msgs
CFLAGS += -isystem microros-lib/include/rosidl_typesupport_introspection_c
CFLAGS += -isystem microros-lib/include/uxr
CFLAGS += -isystem microros-lib/include/unique_identifier_msgs
CFLAGS += -isystem microros-lib/include/rosidl_typesupport_microxrcedds_c
CFLAGS += -isystem microros-lib/include/builtin_interfaces
CFLAGS += -isystem microros-lib/include/tracetools
CFLAGS += -isystem microros-lib/include/rmw_microros
endif

ifneq ($(CIRCUITPY_ESPIDF),0)
SRC_ESPIDF := \
	$(wildcard common-hal/espidf/*.c) \
	$(wildcard bindings/espidf/*.c)
SRC_C += $(SRC_ESPIDF)
endif

ifneq ($(CIRCUITPY_ESPNOW),0)
SRC_ESPNOW := \
	$(wildcard common-hal/espnow/*.c) \
	$(wildcard bindings/espnow/*.c)
SRC_C += $(SRC_ESPNOW)
endif

ifneq ($(CIRCUITPY_ESPULP),0)
SRC_ULP := \
	$(wildcard common-hal/espulp/*.c) \
	$(wildcard bindings/espulp/*.c)
SRC_C += $(SRC_ULP)
endif

ifneq ($(CIRCUITPY_JPEGIO_HW),0)
SRC_C += common-hal/jpegio/JpegDecoder.c
CHIP_COMPONENTS += esp_driver_jpeg esp_hal_jpeg
endif

ifneq ($(CIRCUITPY_NEOPIXEL_WRITE),0)
CHIP_COMPONENTS += esp_driver_rmt
endif

ifneq ($(CIRCUITPY_PARALLELDISPLAYBUS),0)
CHIP_COMPONENTS += esp_driver_i2s
endif

ifneq ($(CIRCUITPY_PULSEIO),0)
CHIP_COMPONENTS += esp_driver_rmt
endif

ifneq ($(CIRCUITPY_COUNTIO),0)
CHIP_COMPONENTS += esp_driver_pcnt
endif

ifneq ($(CIRCUITPY_ROTARYIO),0)
CHIP_COMPONENTS += esp_driver_pcnt
endif

ifneq ($(CIRCUITPY_FREQUENCYIO),0)
CHIP_COMPONENTS += esp_driver_pcnt
endif

ifneq ($(FROZEN_MPY_DIR),)
FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))
endif

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(OBJ_CRT_BUNDLE)

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

$(BUILD)/lib/protomatter/src/core.o: CFLAGS += -DESP32

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED) $(SRC_CIRCUITPY_COMMON)

# IDF build commands
IDF_PATH = $(realpath ./esp-idf)
# create the directory
$(BUILD)/esp-idf:
	$(Q)$(MKDIR) -p $@

TARGET_SDKCONFIG = esp-idf-config/sdkconfig-$(IDF_TARGET).defaults


# Choose the correct partition layout, based on UF2 or not, flash size, etc.
UF2_BOOTLOADER ?= $(CIRCUITPY_USB_DEVICE)

ifeq ($(CIRCUITPY_ESP_FLASH_SIZE), 2MB)
# ..... No 2MB UF2 boards, so always no OTA, not UF2
	FLASH_SIZE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_SIZE)-no-ota-no-uf2.defaults
else ifeq ($(CIRCUITPY_ESP_FLASH_SIZE), 4MB)
	ifeq ($(UF2_BOOTLOADER), 1)
#.............  ESP32-S2 and ESP32-S3 do not have an extra ota partition for OTA update.
		FLASH_SIZE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_SIZE)-no-ota.defaults
	else
# ............. Non-UF2 4MB: C3, C6, etc.
		ifeq ($(CIRCUITPY_4MB_FLASH_LARGE_USER_FS_LAYOUT), 1)
# ....................  Some boards use a 2MB firmware and almost 2MB user filesystem, for historical reasons.
			FLASH_SIZE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_SIZE)-no-ota-no-uf2-large-user_fs.defaults
		else
			FLASH_SIZE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_SIZE)-no-ota-no-uf2.defaults
		endif
	endif
else
# ..... All the rest are >4MB flash
	ifeq ($(UF2_BOOTLOADER), 1)
		FLASH_SIZE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_SIZE).defaults
	else
		FLASH_SIZE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_SIZE)-no-uf2.defaults
	endif
endif

FLASH_MODE_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_MODE).defaults
FLASH_SPEED_SDKCONFIG ?= esp-idf-config/sdkconfig-flash-$(CIRCUITPY_ESP_FLASH_FREQ).defaults

ifneq ($(CIRCUITPY_ESP_PSRAM_SIZE), 0)
	PSRAM_SDKCONFIG = esp-idf-config/sdkconfig-psram.defaults
	PSRAM_SIZE_SDKCONFIG = esp-idf-config/sdkconfig-psram-$(CIRCUITPY_ESP_PSRAM_SIZE).defaults
	PSRAM_MODE_SDKCONFIG = esp-idf-config/sdkconfig-psram-$(CIRCUITPY_ESP_PSRAM_MODE).defaults
	PSRAM_SPEED_SDKCONFIG = esp-idf-config/sdkconfig-psram-$(CIRCUITPY_ESP_PSRAM_FREQ).defaults
endif

ifeq ($(DEBUG), 1)
	DEBUG_SDKCONFIG = esp-idf-config/sdkconfig-debug.defaults
else
	DEBUG_SDKCONFIG = esp-idf-config/sdkconfig-opt.defaults
endif

ifeq ($(ENABLE_JTAG), 1)
	CFLAGS += -DENABLE_JTAG=1
endif

ifneq ($(CIRCUITPY_BLEIO_NATIVE),0)
	BLE_SDKCONFIG := ;esp-idf-config/sdkconfig-ble.defaults
endif

# Loaded native code executes from RAM, which memory protection forbids.
ifneq (,$(filter 1,$(CIRCUITPY_ENABLE_MPY_NATIVE) $(CIRCUITPY_LOAD_NATIVE)))
	NATIVE_SDKCONFIG := ;esp-idf-config/sdkconfig-native.defaults
endif

SDKCONFIGS := esp-idf-config/sdkconfig.defaults;$(DEBUG_SDKCONFIG);$(FLASH_SIZE_SDKCONFIG);$(FLASH_MODE_SDKCONFIG);$(FLASH_SPEED_SDKCONFIG);$(PSRAM_SDKCONFIG);$(PSRAM_SIZE_SDKCONFIG);$(PSRAM_MODE_SDKCONFIG);$(PSRAM_SPEED_SDKCONFIG);$(BLE_SDKCONFIG)$(NATIVE_SDKCONFIG);$(TARGET_SDKCONFIG);boards/$(BOARD)/sdkconfig

# create the config headers
.PHONY: do-sdkconfig
do-sdkconfig: $(BUILD)/esp-idf/config/sdkconfig.h
QSTR_GLOBAL_REQUIREMENTS += $(BUILD)/esp-idf/config/sdkconfig.h
$(BUILD)/esp-idf/config/sdkconfig.h: boards/$(BOARD)/sdkconfig boards/$(BOARD)/mpconfigboard.mk CMakeLists.txt | $(BUILD)/esp-idf
	$(STEPECHO) "Create $@"
	$(Q)env IDF_PATH=$(IDF_PATH) IDF_COMPONENT_MANAGER=0 cmake -S . -B $(BUILD)/esp-idf -DSDKCONFIG=$(BUILD)/esp-idf/sdkconfig -DSDKCONFIG_DEFAULTS="$(SDKCONFIGS)" -DCMAKE_TOOLCHAIN_FILE=$(IDF_PATH)/tools/cmake/toolchain-$(IDF_TARGET).cmake -DIDF_TARGET=$(IDF_TARGET) -DCIRCUITPY_ESP_HOSTED=$(CIRCUITPY_ESP_HOSTED) -DCIRCUITPY_MIPICSI=$(CIRCUITPY_MIPICSI) -GNinja
	$(Q)$(PYTHON) tools/check-sdkconfig.py \
		CIRCUITPY_DUALBANK=$(CIRCUITPY_DUALBANK) \
		CIRCUITPY_STORAGE_EXTEND=$(CIRCUITPY_STORAGE_EXTEND) \
		CIRCUITPY_ENABLE_MPY_NATIVE=$(CIRCUITPY_ENABLE_MPY_NATIVE) \
		CIRCUITPY_LOAD_NATIVE=$(CIRCUITPY_LOAD_NATIVE) \
		$@

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
menuconfig: $(BUILD)/esp-idf/config $(BUILD)/esp-idf/config/sdkconfig.h
	$(Q)ninja -C $(BUILD)/esp-idf menuconfig
	python tools/update_sdkconfig.py --board=$(BOARD) --debug=$(DEBUG)

update-all-sdkconfigs: $(BUILD)/esp-idf/config/sdkconfig.h
	python tools/update_sdkconfig.py --update_all --board=$(BOARD) --debug=$(DEBUG)

update-board-sdkconfig: $(BUILD)/esp-idf/config/sdkconfig.h
	python tools/update_sdkconfig.py --board=$(BOARD) --debug=$(DEBUG)

BINARY_WIFI_BLOBS = libcore.a libespnow.a libnet80211.a libpp.a libsmartconfig.a

ifneq ($(IDF_TARGET),esp32p4)
	BINARY_BLOBS = esp-idf/components/esp_phy/lib/$(IDF_TARGET)/libphy.a
endif
ifneq ($(CIRCUITPY_ESP_HOSTED),1)
ifneq ($(CIRCUITPY_WIFI),0)
	BINARY_BLOBS += $(addprefix esp-idf/components/esp_wifi/lib/$(IDF_TARGET)/, $(BINARY_WIFI_BLOBS))
	BINARY_BLOBS += esp-idf/components/esp_coex/lib/$(IDF_TARGET)/libcoexist.a
	ifneq ($(IDF_TARGET),esp32c2)
	BINARY_BLOBS += $(addprefix esp-idf/components/esp_wifi/lib/$(IDF_TARGET)/, libmesh.a libwapi.a)
	endif
endif

endif

ifeq ($(IDF_TARGET),esp32)
BINARY_BLOBS += esp-idf/components/esp_phy/lib/$(IDF_TARGET)/librtc.a
endif

# CONFIG_COMPILER_CXX_EXCEPTIONS is off, so IDF's cxx component replaces these
# with stubs that abort, and C++ code such as nvs_flash does not link libgcc's
# unwinder. Same list as esp-idf/components/cxx/CMakeLists.txt.
CXX_EXCEPTION_WRAPS = \
	__register_frame_info_bases \
	__register_frame_info \
	__register_frame \
	__register_frame_info_table_bases \
	__register_frame_info_table \
	__register_frame_table \
	__deregister_frame_info_bases \
	__deregister_frame_info \
	_Unwind_Find_FDE \
	_Unwind_GetGR \
	_Unwind_GetCFA \
	_Unwind_GetIP \
	_Unwind_GetIPInfo \
	_Unwind_GetRegionStart \
	_Unwind_GetDataRelBase \
	_Unwind_GetTextRelBase \
	_Unwind_SetIP \
	_Unwind_SetGR \
	_Unwind_GetLanguageSpecificData \
	_Unwind_FindEnclosingFunction \
	_Unwind_Resume \
	_Unwind_RaiseException \
	_Unwind_DeleteException \
	_Unwind_ForcedUnwind \
	_Unwind_Resume_or_Rethrow \
	_Unwind_Backtrace \
	__cxa_call_unexpected \
	__gxx_personality_v0 \
	__cxa_throw \
	__cxa_allocate_exception
LDFLAGS += $(foreach f,$(CXX_EXCEPTION_WRAPS),-Wl,--wrap=$(f))

ESP_IDF_COMPONENTS_LINK = $(IDF_TARGET_ARCH) $(CHIP_COMPONENTS) app_update bootloader_support driver esp_driver_dma esp_driver_gpio esp_driver_gptimer esp_driver_i2c esp_driver_ledc esp_driver_spi esp_driver_uart efuse esp_adc esp_app_format esp_common esp_event esp_gdbstub esp_hal_ana_conv esp_hal_clock esp_hal_gpio esp_hal_gpspi esp_hal_i2c esp_hal_ledc esp_hal_mspi esp_hal_security esp_hal_timg esp_hal_uart esp_hal_wdt esp_hw_support esp_mm esp_partition esp_pm esp_ringbuf esp_rom esp_system esp_timer freertos hal heap log esp_libc nvs_flash nvs_sec_provider pthread soc spi_flash vfs esp_stdio cxx
ifneq ($(IDF_TARGET),esp32)
ESP_IDF_COMPONENTS_LINK += esp_hal_systimer
endif
# These chips have regi2c in ROM. The rest link the esp_hal_regi2c implementation.
ifneq ($(filter $(IDF_TARGET),esp32 esp32c2 esp32c3 esp32s3),)
LDFLAGS += -Trom.regi2c.ld
else
ESP_IDF_COMPONENTS_LINK += esp_hal_regi2c
endif
# Only targets with EMAC support (per SOC_EMAC_SUPPORTED in soc_caps.h) build
# libesp_hal_emac.a; those targets list it in their CHIP_COMPONENTS above. The
# component is interface-only elsewhere, so it must not appear in the list of
# components to link.
ifneq ($(CIRCUITPY_WIFI),0)
	ESP_IDF_COMPONENTS_LINK += esp_netif esp_security esp-tls esp_wifi lwip mbedtls mdns
ifneq ($(CIRCUITPY_ESP_HOSTED),1)
ESP_IDF_COMPONENTS_LINK += esp_coex esp_phy wpa_supplicant
endif
endif
ifeq ($(CIRCUITPY_ESP_HOSTED),1)
# ESP-IDF v6.1+ has remote Wi-Fi support built into esp_wifi, so no separate
# esp_wifi_remote component is needed.
INC := -isystem esp-hosted/host/api/include -isystem esp-hosted/host $(INC)
ESP_IDF_COMPONENTS_LINK += esp-hosted esp_http_client http_parser console
REGISTRATION_FUNCTIONS += -u esp_hosted_init
endif
# mdns registers handlers on ETH_EVENT, which is defined by esp_eth. On EMAC-capable
# targets, esp_eth's compiled EMAC sources reference the esp_hal_emac symbols; that
# library comes via CHIP_COMPONENTS.
ifneq ($(CIRCUITPY_MDNS),0)
	ESP_IDF_COMPONENTS_LINK += esp_eth
endif
ifneq ($(CIRCUITPY_MII),0)
	ESP_IDF_COMPONENTS_LINK += esp_eth
  ifeq ($(CIRCUITPY_WIFI),0)
	ESP_IDF_COMPONENTS_LINK += esp_netif lwip mbedtls
  endif
endif
ifneq ($(CIRCUITPY_BLEIO_NATIVE),0)
	BLE_IMPL_esp32 := esp32
	BLE_IMPL_esp32s3 := esp32c3
	BLE_IMPL_esp32c2 := libble
	BLE_IMPL_esp32c3 := esp32c3
	BLE_IMPL_esp32c5 := libble
	BLE_IMPL_esp32c6 := libble
	BLE_IMPL_esp32c61 := libble
	BLE_IMPL_esp32h2 := libble
	BLE_IMPL = $(BLE_IMPL_$(IDF_TARGET))

	ESP_IDF_COMPONENTS_LINK += bt esp_phy esp_security
	ifeq ($(BLE_IMPL),esp32)
#		BLE will hang the ESP32 and trigger an interrupt watchdog without this undefined symbol at
#		link because a weak version of the interrupt that BLE uses will be linked incorrectly.
		REGISTRATION_FUNCTIONS += -u ld_include_hli_vectors_bt
		BINARY_BLOBS += esp-idf/components/bt/controller/lib_esp32/$(IDF_TARGET)/libbtdm_app.a
	endif

	ifeq ($(BLE_IMPL),esp32c3)
	BINARY_BLOBS += esp-idf/components/esp_phy/lib/$(IDF_TARGET)/libbtbb.a \
					esp-idf/components/bt/controller/lib_esp32c3_family/$(IDF_TARGET)/libbtdm_app.a
	endif

	ifeq ($(BLE_IMPL),libble)
	BINARY_BLOBS += esp-idf/components/esp_phy/lib/$(IDF_TARGET)/libbtbb.a
	# Shared lib first, else the per-chip one.
	BLE_LIB_DIR = esp-idf/components/bt/controller/lib_$(BT_IDF_TARGET)/$(BT_IDF_TARGET)-bt-lib
	BINARY_BLOBS += $(firstword $(wildcard $(BLE_LIB_DIR)/libble_app.a) $(BLE_LIB_DIR)/$(IDF_TARGET)/libble_app.a)
	endif
endif
ifneq ($(filter $(IDF_TARGET),esp32p4 esp32s31),)
    ESP_IDF_COMPONENTS_LINK += esp_security
endif
ifneq ($(CIRCUITPY_ESPULP),0)
	ESP_IDF_COMPONENTS_LINK += ulp
endif
ifneq ($(CIRCUITPY_ESP_PSRAM_SIZE),0)
	ESP_IDF_COMPONENTS_LINK += esp_psram
endif
ifneq ($(CIRCUITPY_DOTCLOCKFRAMEBUFFER),0)
	ESP_IDF_COMPONENTS_LINK += esp_lcd
endif
ifneq ($(CIRCUITPY_MIPIDSI),0)
	ESP_IDF_COMPONENTS_LINK += esp_lcd
endif
ifneq ($(CIRCUITPY_MIPICSI),0)
	ESP_IDF_COMPONENTS_LINK += esp_driver_cam esp_driver_isp esp_hal_cam
endif
ifneq ($(CIRCUITPY_PARALLELDISPLAYBUS),0)
	ESP_IDF_COMPONENTS_LINK += esp_lcd
endif
ifneq ($(CIRCUITPY_QSPIBUS),0)
	ESP_IDF_COMPONENTS_LINK += esp_lcd
endif
ifneq ($(CIRCUITPY_SDIOIO),0)
	ESP_IDF_COMPONENTS_LINK += sdmmc esp_driver_sdmmc esp_driver_sd_intf esp_hal_sd
endif

ESP_IDF_COMPONENTS_EXPANDED = $(foreach component, $(ESP_IDF_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/$(component)/lib$(component).a)

MBEDTLS_COMPONENTS_LINK = crypto tls x509
MBEDTLS_COMPONENTS_LINK_EXPANDED = $(foreach component, $(MBEDTLS_COMPONENTS_LINK), $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/library/libmbed$(component).a)
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/library/libtfpsacrypto.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/libmbed-builtin.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/drivers/everest/libeverest.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/drivers/p256-m/libp256-m.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/extras/libextras.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/platform/libplatform.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/mbedtls/tf-psa-crypto/utilities/libutilities.a
MBEDTLS_COMPONENTS_LINK_EXPANDED += $(BUILD)/esp-idf/esp-idf/mbedtls/libmbedtls.a

ifeq ($(IDF_TARGET_ARCH),xtensa)
BINARY_BLOBS += esp-idf/components/xtensa/$(IDF_TARGET)/libxt_hal.a
ESP_IDF_COMPONENTS_EXPANDED += esp-idf/components/xtensa/$(IDF_TARGET)/libxt_hal.a
endif

ifneq ($(CIRCUITPY_ESPCAMERA),0)
ESP_IDF_COMPONENTS_EXPANDED += $(BUILD)/esp-idf/esp-idf/esp-camera/libesp-camera.a
#$(error $(ESP_IDF_COMPONENTS_EXPANDED))
endif

ifneq ($(CIRCUITPY_RCLCPY),0)
	ifeq ($(IDF_TARGET),esp32)
		BINARY_BLOBS += microros-lib/esp32/libmicroros.a
	else ifeq ($(IDF_TARGET),esp32s2)
		BINARY_BLOBS+= microros-lib/esp32s2/libmicroros.a
	else ifeq ($(IDF_TARGET),esp32s3)
		BINARY_BLOBS += microros-lib/esp32s3/libmicroros.a
	else
		$(error Unsupported Espressif target for Micro-ROS: $(IDF_TARGET).)
	endif
endif

ifneq ($(VALID_BOARD),)
# From esp-idf/components/bootloader/Kconfig.projbuild
# BOOTLOADER_OFFSET is determined by chip type, based on the ROM bootloader, and is not changeable.
ifeq ($(IDF_TARGET),esp32)
BOOTLOADER_OFFSET = 0x1000
else ifeq ($(IDF_TARGET),esp32h2)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32c2)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32c3)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32c5)
BOOTLOADER_OFFSET = 0x2000
else ifeq ($(IDF_TARGET),esp32c6)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32c61)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32p4)
BOOTLOADER_OFFSET = 0x2000
else ifeq ($(IDF_TARGET),esp32s31)
BOOTLOADER_OFFSET = 0x2000
else ifeq ($(IDF_TARGET),esp32s3)
BOOTLOADER_OFFSET = 0x0
else ifeq ($(IDF_TARGET),esp32s2)
BOOTLOADER_OFFSET = 0x1000
else
$(error unknown IDF_TARGET $(IDF_TARGET))
endif
endif

IDF_CMAKE_TARGETS = \
		bootloader/bootloader.bin \
		__ldgen_output_sections.ld \
		$(foreach component, $(ESP_IDF_COMPONENTS_LINK), esp-idf/$(component)/lib$(component).a)

PARTITION_TABLE_OFFSET = 0x8000
FIRMWARE_OFFSET = 0x10000

# Map the flash's run mode to flashing mode (only dual is supported by the early bootloaders).
# https://github.com/espressif/esp-idf/blob/82cceabc6e6a0a2d8c40e2249bc59917cc5e577a/components/esptool_py/Kconfig.projbuild#L74-L87
ifeq ($(CIRCUITPY_ESP_FLASH_MODE),dout)
	ESPTOOLPY_FLASHMODE = "dout"
endif
ifeq ($(CIRCUITPY_ESP_FLASH_MODE),opi)
	ESPTOOLPY_FLASHMODE = "dout"
endif
# Most modes can flash with dio
ESPTOOLPY_FLASHMODE ?= "dio"

# Cap the flash speed to 80m.
# https://github.com/espressif/esp-idf/blob/82cceabc6e6a0a2d8c40e2249bc59917cc5e577a/components/esptool_py/Kconfig.projbuild#L74-L87
ifeq ($(CIRCUITPY_ESP_FLASH_FREQ),120m)
	ESPTOOLPY_FLASHFREQ = "80m"
else
	ESPTOOLPY_FLASHFREQ = $(CIRCUITPY_ESP_FLASH_FREQ)
endif

FLASH_FLAGS = --flash-mode $(ESPTOOLPY_FLASHMODE) --flash-freq $(ESPTOOLPY_FLASHFREQ) --flash-size $(CIRCUITPY_ESP_FLASH_SIZE)

ESPTOOL_FLAGS ?= --before=default-reset --after=no-reset --baud 921600

ifeq ($(UF2_BOOTLOADER),1)
all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2
else
all: $(BUILD)/firmware.bin
endif

$(IDF_CMAKE_TARGETS): esp-idf-stamp

.PHONY: esp-idf-stamp
esp-idf-stamp: $(BUILD)/esp-idf/config/sdkconfig.h
	$(Q)ninja -C $(BUILD)/esp-idf $(IDF_CMAKE_TARGETS)

$(BUILD)/firmware.elf: $(OBJ) | esp-idf-stamp $(IDF_CMAKE_TARGETS)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--print-memory-usage -Wl,--start-group $(ESP_IDF_COMPONENTS_EXPANDED) $(BINARY_BLOBS) $(MBEDTLS_COMPONENTS_LINK_EXPANDED) $(LIBS) -Wl,--end-group $(REGISTRATION_FUNCTIONS)

$(BUILD)/circuitpython-firmware.bin: $(BUILD)/firmware.elf | tools/build_memory_info.py
	$(STEPECHO) "Create $@"
	$(Q)esptool --chip $(IDF_TARGET) elf2image $(FLASH_FLAGS) --elf-sha256-offset 0xb0 -o $@ $^
	$(Q)$(PYTHON) tools/build_memory_info.py $< $(BUILD)/esp-idf/sdkconfig $@ $(BUILD)

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.bin: invalid-board
else
$(BUILD)/firmware.bin: $(BUILD)/circuitpython-firmware.bin | esp-idf-stamp
	$(Q)$(PYTHON) ../../tools/join_bins.py $@ $(BOOTLOADER_OFFSET) $(BUILD)/esp-idf/bootloader/bootloader.bin $(PARTITION_TABLE_OFFSET) $(BUILD)/esp-idf/partition_table/partition-table.bin $(FIRMWARE_OFFSET) $(BUILD)/circuitpython-firmware.bin
endif

UF2_FAMILY_ID_esp32s2 = 0xbfdd4eee
UF2_FAMILY_ID_esp32s3 = 0xc47e5767
UF2_FAMILY_ID_esp32p4 = 0x3d308e94

$(BUILD)/firmware.uf2: $(BUILD)/circuitpython-firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -f $(UF2_FAMILY_ID_$(IDF_TARGET)) -b 0x0000 -c -o $@ $^

flash: $(BUILD)/firmware.bin
	esptool --chip $(IDF_TARGET) -p $(PORT) $(ESPTOOL_FLAGS) write-flash $(FLASH_FLAGS) 0x0000 $^

flash-circuitpython-only: $(BUILD)/circuitpython-firmware.bin
	esptool --chip $(IDF_TARGET) -p $(PORT) $(ESPTOOL_FLAGS) write-flash $(FLASH_FLAGS) $(FIRMWARE_OFFSET) $^

monitor: $(BUILD)/firmware.elf
	cp $< build/circuitpython.elf
	idf.py monitor -p $(PORT)

include $(TOP)/py/mkrules.mk
