# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

include ../../py/circuitpy_mkenv.mk

CROSS_COMPILE = arm-none-eabi-

HAL_DIR=hal/$(MCU_SERIES)

ifeq ($(CIRCUITPY_CYW43),1)
INC_CYW43 := \
	-isystem lib/cyw43-driver \
	-isystem lib/cyw43-driver/firmware \
	-isystem lib/cyw43-driver/src \
	-isystem lib/lwip/src/include \
	-isystem sdk/src/rp2_common/pico_async_context/include/ \
	-isystem sdk/src/rp2_common/pico_cyw43_arch/include/ \
	-isystem sdk/src/rp2_common/pico_cyw43_driver/include/ \
	-isystem sdk/src/rp2_common/pico_lwip/include/ \
	-isystem sdk/src/rp2_common/pico_rand/include/ \

CFLAGS_CYW43 := \
	-DCYW43_LWIP=1 \
	-DPICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1 \
	-DCYW43_USE_SPI=1 \
	-DUSE_SDIOIT=0 \
	-DIGNORE_GPIO25 \
	-DIGNORE_GPIO23 \
	-DIGNORE_GPIO24 \
	-DCYW43_LOGIC_DEBUG=0 \
	-DCYW43_USE_STATS=0 \
	-DPICO_BUILD \
	-DCYW43_ENABLE_BLUETOOTH=0 \
	-DCIRCUITPY_CYW43_INIT_DELAY=$(CIRCUITPY_CYW43_INIT_DELAY) \
	-DPICO_CYW43_ARCH_POLL=0

SRC_SDK_CYW43 := \
	src/common/pico_sync/sem.c \
	src/rp2_common/pico_async_context/async_context_base.c \
	src/rp2_common/pico_async_context/async_context_threadsafe_background.c \
	src/rp2_common/pico_cyw43_arch/cyw43_arch.c \
	src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background.c \
	src/rp2_common/pico_cyw43_driver/cyw43_driver.c \
	src/rp2_common/pico_cyw43_driver/cyw43_bus_pio_spi.c \
	src/rp2_common/pico_lwip/lwip_nosys.c \
	src/rp2_common/pico_rand/rand.c \

SRC_LWIP := \
	shared/netutils/netutils.c \
	shared/netutils/trace.c \
	shared/netutils/dhcpserver.c \
	$(wildcard lib/lwip/src/apps/mdns/*.c) \
	$(wildcard lib/lwip/src/core/*.c) \
	$(wildcard lib/lwip/src/core/ipv4/*.c) \
	lib/lwip/src/netif/ethernet.c \
	$(wildcard lwip_src/*.c) \

SRC_CYW43 := \
	$(wildcard bindings/cyw43/*.c) \
	lib/cyw43-driver/src/cyw43_stats.c \
	lib/cyw43-driver/src/cyw43_ctrl.c \
	lib/cyw43-driver/src/cyw43_ll.c \
	lib/cyw43-driver/src/cyw43_lwip.c \

PIOASM = $(BUILD)/pioasm/pioasm/pioasm
.PHONY: pioasmBuild
pioasmBuild: $(PIOASM)
$(PIOASM):
	$(Q)cmake -S pioasm -B $(BUILD)/pioasm
	$(Q)$(MAKE) -C $(BUILD)/pioasm pioasmBuild

$(BUILD)/cyw43_bus_pio_spi.pio.h: sdk/src/rp2_common/pico_cyw43_driver/cyw43_bus_pio_spi.pio $(PIOASM)
	$(Q)$(PIOASM) -o c-sdk $< $@
$(BUILD)/sdk/src/rp2_common/pico_cyw43_driver/cyw43_bus_pio_spi.o: $(BUILD)/cyw43_bus_pio_spi.pio.h

else
INC_CYW43 :=
CFLAGS_CYW43 :=
SRC_SDK_CYW43 :=
SRC_CYW43 :=
SRC_LWIP :=
endif

ifeq ($(CIRCUITPY_WIZNET), 1)
INC_WIZNET := \
	-isystem common-hal/wiznet \

SRC_WIZNET := \
	common-hal/wiznet/wizchip_pio_spi.c \

WIZNET_PIOASM = $(BUILD)/pioasm/pioasm/pioasm
.PHONY: pioasmBuild
pioasmBuild: $(WIZNET_PIOASM)

$(WIZNET_PIOASM):
	$(Q)cmake -S pioasm -B $(BUILD)/pioasm
	$(MAKE) -C $(BUILD)/pioasm pioasmBuild

$(BUILD)/wizchip_pio_spi.pio.h: common-hal/wiznet/wizchip_pio_spi.pio $(WIZNET_PIOASM)
	$(Q)$(WIZNET_PIOASM) -o c-sdk $< $@
$(BUILD)/common-hal/wiznet/wizchip_pio_spi.o: $(BUILD)/wizchip_pio_spi.pio.h

$(BUILD)/genhdr/qstr.i.last: $(BUILD)/wizchip_pio_spi.pio.h

else
INC_WIZNET :=
SRC_WIZNET :=

endif

CHIP_VARIANT_LOWER = $(shell echo $(CHIP_VARIANT) | tr '[:upper:]' '[:lower:]')

INC += \
	-I. \
	-Ilwip_inc \
	-I../.. \
	-I../lib/mp-readline \
	-I../shared/timeutils \
	-Iboards/$(BOARD) \
	-Iboards/ \
	-isystem sdk/src/common/boot_picobin_headers/include/ \
	-isystem sdk/src/common/boot_picoboot_headers/include/ \
	-isystem sdk/src/common/hardware_claim/include/ \
	-isystem sdk/src/common/pico_base_headers/include/ \
	-isystem sdk/src/common/pico_binary_info/include/ \
	-isystem sdk/src/common/pico_stdlib_headers/include/ \
	-isystem sdk/src/common/pico_sync/include/ \
	-isystem sdk/src/common/pico_time/include/ \
	-isystem sdk/src/common/pico_util/include/ \
	-isystem sdk/src/$(CHIP_VARIANT_LOWER)/hardware_regs/include/ \
	-isystem sdk/src/$(CHIP_VARIANT_LOWER)/hardware_structs/include/ \
	-isystem sdk/src/$(CHIP_VARIANT_LOWER)/pico_platform/include/ \
	-isystem sdk/src/rp2_common/boot_bootrom_headers/include/ \
	-isystem sdk/src/rp2_common/cmsis/stub/CMSIS/Core/Include/ \
	-isystem sdk/src/rp2_common/cmsis/stub/CMSIS/Device/${CHIP_VARIANT}/Include \
	-isystem sdk/src/rp2_common/cmsis/ \
	-isystem sdk/src/rp2_common/hardware_adc/include/ \
	-isystem sdk/src/rp2_common/hardware_base/include/ \
	-isystem sdk/src/rp2_common/hardware_boot_lock/include/ \
	-isystem sdk/src/rp2_common/hardware_clocks/include/ \
	-isystem sdk/src/rp2_common/hardware_divider/include/ \
	-isystem sdk/src/rp2_common/hardware_dma/include/ \
	-isystem sdk/src/rp2_common/hardware_flash/include/ \
	-isystem sdk/src/rp2_common/hardware_gpio/include/ \
	-isystem sdk/src/rp2_common/hardware_interp/include/ \
	-isystem sdk/src/rp2_common/hardware_irq/include/ \
	-isystem sdk/src/rp2_common/hardware_i2c/include/ \
	-isystem sdk/src/rp2_common/hardware_pio/include/ \
	-isystem sdk/src/rp2_common/hardware_pll/include/ \
	-isystem sdk/src/rp2_common/hardware_powman/include/ \
	-isystem sdk/src/rp2_common/hardware_pwm/include/ \
	-isystem sdk/src/rp2_common/hardware_resets/include/ \
	-isystem sdk/src/rp2_common/hardware_rcp/include/ \
	-isystem sdk/src/rp2_common/hardware_rtc/include/ \
	-isystem sdk/src/rp2_common/hardware_spi/include/ \
	-isystem sdk/src/rp2_common/hardware_sync/include/ \
	-isystem sdk/src/rp2_common/hardware_sync_spin_lock/include/ \
	-isystem sdk/src/rp2_common/hardware_ticks/include/ \
	-isystem sdk/src/rp2_common/hardware_timer/include/ \
	-isystem sdk/src/rp2_common/hardware_uart/include/ \
	-isystem sdk/src/rp2_common/hardware_vreg/include/ \
	-isystem sdk/src/rp2_common/hardware_watchdog/include/ \
	-isystem sdk/src/rp2_common/hardware_xosc/include/ \
	-isystem sdk/src/rp2_common/hardware_xip_cache/include/ \
	-isystem sdk/src/rp2_common/pico_aon_timer/include/ \
	-isystem sdk/src/rp2_common/pico_atomic/include/ \
	-isystem sdk/src/rp2_common/pico_bootrom/include/ \
	-isystem sdk/src/rp2_common/pico_double/include/ \
	-isystem sdk/src/rp2_common/pico_flash/include/ \
	-isystem sdk/src/rp2_common/pico_mem_ops/include/ \
	-isystem sdk/src/rp2_common/pico_multicore/include/ \
	-isystem sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include/ \
	-isystem sdk/src/rp2_common/pico_stdio/include/ \
	-isystem sdk/src/rp2_common/pico_printf/include/ \
	-isystem sdk/src/rp2_common/pico_float/include/ \
	-isystem sdk/src/rp2_common/pico_runtime/include/ \
	-isystem sdk/src/rp2_common/pico_runtime_init/include/ \
	-isystem sdk/src/rp2_common/pico_platform_common/include/ \
	-isystem sdk/src/rp2_common/pico_platform_compiler/include/ \
	-isystem sdk/src/rp2_common/pico_platform_sections/include/ \
	-isystem sdk/src/rp2_common/pico_platform_panic/include/ \
	-isystem sdk/src/rp2_common/pico_time_adapter/include/ \
	-isystem sdk/src/rp2_common/pico_unique_id/include/ \
	$(INC_CYW43) \
	-Isdk_config \
	-I../../lib/tinyusb/src \
	-I../../supervisor/shared/usb \
	-I$(BUILD)

# Pico specific configuration
CFLAGS += \
	-DRASPBERRYPI \
	-DLIB_PICO_MULTICORE=1 \
	-DPICO_NO_HARDWARE=0 \
	-DPICO_ON_DEVICE=1 \
	-DPICO_BUILD=1 \
	-DPICO_NO_BINARY_INFO=0 \
	-DPICO_TIME_DEFAULT_ALARM_POOL_DISABLED=0 \
	-DPICO_DIVIDER_CALL_IDIV0=0 \
	-DPICO_DIVIDER_CALL_LDIV0=0 \
	-DPICO_DIVIDER_HARDWARE=1 \
	-DPICO_DOUBLE_ROM=1 \
	-DPICO_FLOAT_ROM=1 \
	-DPICO_BITS_IN_RAM=0 \
	-DPICO_DIVIDER_IN_RAM=1 \
	-DPICO_DOUBLE_PROPAGATE_NANS=0 \
	-DPICO_DOUBLE_IN_RAM=0 \
	-DPICO_MEM_IN_RAM=0 \
	-DPICO_FLOAT_IN_RAM=0 \
	-DPICO_FLOAT_PROPAGATE_NANS=1 \
	-DPICO_NO_FLASH=0 \
	-DPICO_COPY_TO_RAM=0 \
	-DPICO_DISABLE_SHARED_IRQ_HANDLERS=0 \
	-DPICO_NO_BI_BOOTSEL_VIA_DOUBLE_RESET=0 \
	-DDVI_1BPP_BIT_REVERSE=0

# Shared IRQ handlers, counted over all IRQs: USB (2), CYW43, audio, rp2pio, PinAlarm and
# max3421e. The SDK default of 4 is too few on Pico W boards.
CFLAGS += -DPICO_MAX_SHARED_IRQ_HANDLERS=8

# -O2 plus the two -O3 flags that measure: -funswitch-loops (16 KB) carries the sprite
# blits, -fvect-cost-model=dynamic (4 KB) the fills and memcpy. Same speed as -O3 on both
# cores for about 125 KB less flash. Boards may override this in mpconfigboard.mk.
OPTIMIZATION_FLAGS ?= -O2 -funswitch-loops -fvect-cost-model=dynamic
# TinyUSB defines
CFLAGS += \
	-DCFG_TUSB_OS=OPT_OS_PICO \
	-DTUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1 \
	-DCFG_TUSB_MCU=OPT_MCU_RP2040 \
	-DCFG_TUD_MIDI_RX_BUFSIZE=128 \
	-DCFG_TUD_CDC_RX_BUFSIZE=256 \
	-DCFG_TUD_MIDI_TX_BUFSIZE=128 \
	-DCFG_TUD_CDC_TX_BUFSIZE=256 \
	-DCFG_TUD_MSC_BUFSIZE=1024 \
	-DPICO_RP2040_USB_DEVICE_UFRAME_FIX=1 \
	-DPICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 \

# option to override default optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

# flags specific to wifi / cyw43

CFLAGS += $(CFLAGS_CYW43)
#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -ggdb3 -Og
  # No LTO because we may place some functions in RAM instead of flash.
else
  CFLAGS += -DNDEBUG

  # No LTO because we may place some functions in RAM instead of flash.

  ifdef CFLAGS_BOARD
    CFLAGS += $(CFLAGS_BOARD)
  endif
endif

DISABLE_WARNINGS = -Wno-cast-align

CFLAGS += $(INC) -Wtype-limits -Wall -Werror -std=gnu11 -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) $(DISABLE_WARNINGS) -Werror=missing-prototypes -Wold-style-definition

PICO_LDFLAGS = --specs=nosys.specs --specs=nano.specs
# The SDK's panic() is flash-resident, but core1 may run with flash access
# disabled by the MPU. Route panics to the RAM-resident __wrap_panic in
# supervisor/port.c, which halts safely on core1 and prints on core0.
PICO_LDFLAGS += -Wl,--wrap=panic

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

LIBS += -lc

PICO_WRAP_FLOAT_AEABI_FLAGS := \
	-Wl,--wrap=__aeabi_fadd \
	-Wl,--wrap=__aeabi_fdiv \
	-Wl,--wrap=__aeabi_fmul \
	-Wl,--wrap=__aeabi_frsub \
	-Wl,--wrap=__aeabi_fsub \
	-Wl,--wrap=__aeabi_cfcmpeq \
	-Wl,--wrap=__aeabi_cfrcmple \
	-Wl,--wrap=__aeabi_cfcmple \
	-Wl,--wrap=__aeabi_fcmpeq \
	-Wl,--wrap=__aeabi_fcmplt \
	-Wl,--wrap=__aeabi_fcmple \
	-Wl,--wrap=__aeabi_fcmpge \
	-Wl,--wrap=__aeabi_fcmpgt \
	-Wl,--wrap=__aeabi_fcmpun \
	-Wl,--wrap=__aeabi_i2f \
	-Wl,--wrap=__aeabi_l2f \
	-Wl,--wrap=__aeabi_ui2f \
	-Wl,--wrap=__aeabi_ul2f \
	-Wl,--wrap=__aeabi_f2iz \
	-Wl,--wrap=__aeabi_f2lz \
	-Wl,--wrap=__aeabi_f2uiz \
	-Wl,--wrap=__aeabi_f2ulz \
	-Wl,--wrap=__aeabi_f2d \
	-Wl,--wrap=sqrtf

PICO_WRAP_FLOAT_SCI_FLAGS := \
	-Wl,--wrap=cosf \
	-Wl,--wrap=sinf \
	-Wl,--wrap=tanf \
	-Wl,--wrap=atan2f \
	-Wl,--wrap=expf \
	-Wl,--wrap=logf \
	-Wl,--wrap=ldexpf \
	-Wl,--wrap=copysignf \
	-Wl,--wrap=truncf \
	-Wl,--wrap=floorf \
	-Wl,--wrap=ceilf \
	-Wl,--wrap=roundf \
	-Wl,--wrap=sincosf \
	-Wl,--wrap=asinf \
	-Wl,--wrap=acosf \
	-Wl,--wrap=atanf \
	-Wl,--wrap=sinhf \
	-Wl,--wrap=coshf \
	-Wl,--wrap=tanhf \
	-Wl,--wrap=asinhf \
	-Wl,--wrap=acoshf \
	-Wl,--wrap=atanhf \
	-Wl,--wrap=exp2f \
	-Wl,--wrap=log2f \
	-Wl,--wrap=exp10f \
	-Wl,--wrap=log10f \
	-Wl,--wrap=powf \
	-Wl,--wrap=powintf \
	-Wl,--wrap=hypotf \
	-Wl,--wrap=cbrtf \
	-Wl,--wrap=fmodf \
	-Wl,--wrap=dremf \
	-Wl,--wrap=remainderf \
	-Wl,--wrap=remquof \
	-Wl,--wrap=expm1f \
	-Wl,--wrap=log1pf \
	-Wl,--wrap=fmaf

PICO_WRAP_DOUBLE_FLAGS := \
	-Wl,--wrap=__aeabi_dadd \
	-Wl,--wrap=__aeabi_ddiv \
	-Wl,--wrap=__aeabi_dmul \
	-Wl,--wrap=__aeabi_drsub \
	-Wl,--wrap=__aeabi_dsub \
	-Wl,--wrap=__aeabi_cdcmpeq \
	-Wl,--wrap=__aeabi_cdrcmple \
	-Wl,--wrap=__aeabi_cdcmple \
	-Wl,--wrap=__aeabi_dcmpeq \
	-Wl,--wrap=__aeabi_dcmplt \
	-Wl,--wrap=__aeabi_dcmple \
	-Wl,--wrap=__aeabi_dcmpge \
	-Wl,--wrap=__aeabi_dcmpgt \
	-Wl,--wrap=__aeabi_dcmpun \
	-Wl,--wrap=__aeabi_i2d \
	-Wl,--wrap=__aeabi_l2d \
	-Wl,--wrap=__aeabi_ui2d \
	-Wl,--wrap=__aeabi_ul2d \
	-Wl,--wrap=__aeabi_d2iz \
	-Wl,--wrap=__aeabi_d2lz \
	-Wl,--wrap=__aeabi_d2uiz \
	-Wl,--wrap=__aeabi_d2ulz \
	-Wl,--wrap=__aeabi_d2f \
	-Wl,--wrap=sqrt \
	-Wl,--wrap=cos \
	-Wl,--wrap=sin \
	-Wl,--wrap=tan \
	-Wl,--wrap=atan2 \
	-Wl,--wrap=exp \
	-Wl,--wrap=log \
	-Wl,--wrap=ldexp \
	-Wl,--wrap=copysign \
	-Wl,--wrap=trunc \
	-Wl,--wrap=floor \
	-Wl,--wrap=ceil \
	-Wl,--wrap=round \
	-Wl,--wrap=sincos \
	-Wl,--wrap=asin \
	-Wl,--wrap=acos \
	-Wl,--wrap=atan \
	-Wl,--wrap=sinh \
	-Wl,--wrap=cosh \
	-Wl,--wrap=tanh \
	-Wl,--wrap=asinh \
	-Wl,--wrap=acosh \
	-Wl,--wrap=atanh \
	-Wl,--wrap=exp2 \
	-Wl,--wrap=log2 \
	-Wl,--wrap=exp10 \
	-Wl,--wrap=log10 \
	-Wl,--wrap=pow \
	-Wl,--wrap=powint \
	-Wl,--wrap=hypot \
	-Wl,--wrap=cbrt \
	-Wl,--wrap=fmod \
	-Wl,--wrap=drem \
	-Wl,--wrap=remainder \
	-Wl,--wrap=remquo \
	-Wl,--wrap=expm1 \
	-Wl,--wrap=log1p \
	-Wl,--wrap=fma

PICO_WRAP_MEM_OPS_FLAGS := \
	-Wl,--wrap=memcpy \
	-Wl,--wrap=memset \
	-Wl,--wrap=__aeabi_memcpy \
	-Wl,--wrap=__aeabi_memset \
	-Wl,--wrap=__aeabi_memcpy4 \
	-Wl,--wrap=__aeabi_memset4 \
	-Wl,--wrap=__aeabi_memcpy8 \
	-Wl,--wrap=__aeabi_memset8

# Wrap a bunch of math stuff to use the Pico SDK divider
OTHER_PICO_FLAGS := \
	-Wl,--wrap=__aeabi_ldiv0 \
	-Wl,--wrap=__aeabi_idiv0 \
	-Wl,--wrap=__aeabi_lmul \
	-Wl,--wrap=__clzsi2 \
	-Wl,--wrap=__clzdi2 \
	-Wl,--wrap=__ctzsi2 \
	-Wl,--wrap=__ctzdi2 \
	-Wl,--wrap=__popcountsi2 \
	-Wl,--wrap=__popcountdi2 \
	-Wl,--wrap=__clz \
	-Wl,--wrap=__clzl \
	-Wl,--wrap=__clzll \
	-Wl,--wrap=__aeabi_idiv \
	-Wl,--wrap=__aeabi_idivmod \
	-Wl,--wrap=__aeabi_ldivmod \
	-Wl,--wrap=__aeabi_uidiv \
	-Wl,--wrap=__aeabi_uidivmod \
	-Wl,--wrap=__aeabi_uldivmod

SRC_S = shared/runtime/gchelper_thumb1.s

ifeq ($(CHIP_VARIANT),RP2040)
CFLAGS += \
	-march=armv6-m \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft

CFLAGS += -DPICO_RP2040=1 -DPICO_RP2350=0

SRC_SDK_CHIP_VARIANT := \
	src/rp2_common/hardware_rtc/rtc.c \
	src/rp2_common/pico_double/double_init_rom_rp2040.c \
	src/rp2_common/pico_float/float_init_rom_rp2040.c \
	src/rp2_common/pico_mem_ops/mem_ops.c \

SRC_S_UPPER_CHIP_VARIANT := \
	sdk/src/rp2_common/hardware_divider/divider.S \
	sdk/src/rp2_common/pico_divider/divider_hardware.S \
	sdk/src/rp2_common/pico_double/double_v1_rom_shim_rp2040.S \
	sdk/src/rp2_common/pico_float/float_v1_rom_shim_rp2040.S \
	sdk/src/rp2_common/pico_float/float_aeabi_rp2040.S \
	sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S \

PICO_LDFLAGS += \
	$(PICO_WRAP_FLOAT_AEABI_FLAGS) \
	$(PICO_WRAP_FLOAT_SCI_FLAGS) \
	$(PICO_WRAP_DOUBLE_FLAGS) \
	$(PICO_WRAP_MEM_OPS_FLAGS) \
	$(OTHER_PICO_FLAGS)

UF2_ID = 0xE48BFF56

DOUBLE_EABI = rp2040
endif
ifeq ($(CHIP_VARIANT),RP2350)
AFLAGS = -march=armv8-m.main+fp+dsp \
	-mthumb \
	-mcpu=cortex-m33 \
	-mfloat-abi=softfp
CFLAGS += $(AFLAGS) \
	-mabi=aapcs-linux

# ARM Secure family id
UF2_ID = 0xe48bff59

# Double coprocessor is only available on the ARM core.
DOUBLE_EABI = dcp
INC += \
	-isystem sdk/src/rp2_common/hardware_dcp/include/ \
	-isystem sdk/src/rp2_common/hardware_rosc/include/ \
	-isystem sdk/src/rp2_common/pico_low_power/include/

CFLAGS += -DPICO_RP2040=0 -DPICO_RP2350=1

SRC_SDK_CHIP_VARIANT := \
	src/common/pico_util/fixed_bitset.c \
	src/rp2_common/hardware_powman/powman.c \
	src/rp2_common/hardware_rosc/rosc.c \
	src/rp2_common/pico_low_power/low_power.c \

SRC_S_UPPER_CHIP_VARIANT := \
	sdk/src/rp2_common/pico_double/double_conv_m33.S \
	sdk/src/rp2_common/pico_double/double_fma_dcp.S \
	sdk/src/rp2_common/pico_double/double_sci_m33.S \
	sdk/src/rp2_common/pico_float/float_sci_m33_vfp.S \
	sdk/src/rp2_common/pico_float/float_common_m33.S \

PICO_LDFLAGS += $(PICO_WRAP_FLOAT_SCI_FLAGS) $(PICO_WRAP_DOUBLE_FLAGS)

ifeq ($(CHIP_PACKAGE),A)
CFLAGS += -DPICO_RP2350A=1
CFLAGS += -DPICO_RP2350B=0
else
CFLAGS += -DPICO_RP2350A=0
CFLAGS += -DPICO_RP2350B=1
endif

endif

SRC_C += shared/runtime/gchelper_native.c

ifeq ($(CIRCUITPY_PICOGAME),1)
# SIO interpolator fast path for the picogame mode7 inner row (RP2040 + RP2350).
# Portable C remains the fallback on other ports.
SRC_C += common-hal/picogame/interp.c
CFLAGS += -DPICOGAME_HAS_INTERP=1
endif

ifeq ($(CIRCUITPY_BITMAPTOOLS_FAST_ROTOZOOM),1)
SRC_C += common-hal/bitmaptools/__init__.c
endif

SRC_SDK := \
	src/common/hardware_claim/claim.c \
	src/common/pico_sync/critical_section.c \
	src/common/pico_sync/lock_core.c \
	src/common/pico_sync/mutex.c \
	src/common/pico_time/time.c \
	src/common/pico_time/timeout_helper.c \
	src/common/pico_util/datetime.c \
	src/common/pico_util/pheap.c \
	src/common/pico_util/queue.c \
	src/rp2_common/hardware_adc/adc.c \
	src/rp2_common/hardware_clocks/clocks.c \
	src/rp2_common/hardware_dma/dma.c \
	src/rp2_common/hardware_flash/flash.c \
	src/rp2_common/hardware_gpio/gpio.c \
	src/rp2_common/hardware_i2c/i2c.c \
	src/rp2_common/hardware_interp/interp.c \
	src/rp2_common/hardware_irq/irq.c \
	src/rp2_common/hardware_pio/pio.c \
	src/rp2_common/hardware_pll/pll.c \
	src/rp2_common/hardware_spi/spi.c \
	src/rp2_common/hardware_sync/sync.c \
	src/rp2_common/hardware_sync_spin_lock/sync_spin_lock.c \
	src/rp2_common/hardware_ticks/ticks.c \
	src/rp2_common/hardware_timer/timer.c \
	src/rp2_common/hardware_uart/uart.c \
	src/rp2_common/hardware_vreg/vreg.c \
	src/rp2_common/hardware_watchdog/watchdog.c \
	src/rp2_common/hardware_xip_cache/xip_cache.c \
	src/rp2_common/hardware_xosc/xosc.c \
	src/rp2_common/pico_aon_timer/aon_timer.c \
	src/rp2_common/pico_atomic/atomic.c \
	src/rp2_common/pico_bootrom/bootrom.c \
	src/rp2_common/pico_bootsel_via_double_reset/pico_bootsel_via_double_reset.c \
	src/rp2_common/pico_clib_interface/newlib_interface.c \
	src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c \
	src/rp2_common/pico_float/float_math.c \
	src/rp2_common/pico_multicore/multicore.c \
	src/rp2_common/pico_platform_panic/panic.c \
	src/rp2_common/pico_printf/printf.c \
	src/rp2_common/pico_runtime/runtime.c \
	src/rp2_common/pico_runtime_init/runtime_init.c \
	src/rp2_common/pico_runtime_init/runtime_init_clocks.c \
	src/rp2_common/pico_runtime_init/runtime_init_stack_guard.c \
	src/rp2_common/pico_stdio/stdio.c \
	src/rp2_common/pico_stdlib/stdlib.c \
	src/rp2_common/pico_unique_id/unique_id.c \
	src/$(CHIP_VARIANT_LOWER)/pico_platform/platform.c \
	$(SRC_SDK_CYW43) \
	$(SRC_SDK_CHIP_VARIANT) \

SRC_SDK := $(addprefix sdk/, $(SRC_SDK))
$(patsubst %.c,$(BUILD)/%.o,$(SRC_SDK) $(SRC_CYW43)): CFLAGS += -Wno-missing-prototypes -Wno-undef -Wno-unused-function -Wno-nested-externs -Wno-strict-prototypes -Wno-double-promotion -Wno-sign-compare -Wno-unused-variable -Wno-strict-overflow -Ilib/cyw43-driver

SRC_C += \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	common-hal/rtc/pico_time.c \
	bindings/rp2pio/StateMachine.c \
	bindings/rp2pio/__init__.c \
	common-hal/rp2pio/StateMachine.c \
	common-hal/rp2pio/__init__.c \
	bindings/wiznet/PIO_SPI.c \
	bindings/wiznet/__init__.c \
	common-hal/wiznet/PIO_SPI.c \
	common-hal/wiznet/__init__.c \
	audio_dma.c \
	background.c \
	peripherals/pins.c \
	lib/crypto-algorithms/sha256.c \
	lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c \
	lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c \
	mphalport.c \
	$(SRC_CYW43) \
	$(SRC_LWIP) \
	$(SRC_WIZNET) \


ifeq ($(CIRCUITPY_USB_HOST), 1)
SRC_C += \
	lib/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c \
	lib/Pico-PIO-USB/src/pio_usb.c \
	lib/Pico-PIO-USB/src/pio_usb_host.c \
	lib/Pico-PIO-USB/src/usb_crc.c \

INC += \
	-isystem lib/Pico-PIO-USB/src

ifeq ($(CIRCUITPY_USB_HOST_BULK),1)
# Pico-PIO-USB bulk IN ring, polled from the host frame handler.
CFLAGS += -DPIO_USB_HOST_BULK_STREAM=1
endif

# Core1 posts USB events while core0 may hold the TinyUSB queue mutex. The
# contended wait path calls these two flash-resident SDK functions, and core1
# must not execute from flash. Replace them with RAM-resident implementations
# in common-hal/usb_host/Port.c on USB host builds only. See issue #11116.
PICO_LDFLAGS += -Wl,--wrap=best_effort_wfe_or_timeout -Wl,--wrap=time_us_64
endif

ifeq ($(CIRCUITPY_PICODVI),1)
SRC_C += \
	bindings/picodvi/__init__.c \
	bindings/picodvi/AudioOut.c \
	bindings/picodvi/Framebuffer.c \
	common-hal/picodvi/__init__.c \
	common-hal/picodvi/Framebuffer_$(CHIP_VARIANT).c \

ifeq ($(CIRCUITPY_PICODVI_AUDIOOUT),1)
SRC_C += \
	common-hal/picodvi/AudioOut.c \
	common-hal/picodvi/dvi_audio_packet.c \

endif

ifeq ($(CHIP_VARIANT),RP2040)
SRC_PICODVI := \
	lib/PicoDVI/software/libdvi/dvi.c \
	lib/PicoDVI/software/libdvi/dvi_serialiser.c \
	lib/PicoDVI/software/libdvi/dvi_timing.c \
	lib/PicoDVI/software/libdvi/tmds_encode.c \

SRC_C += $(SRC_PICODVI)
$(patsubst %.c,$(BUILD)/%.o,$(SRC_PICODVI))): CFLAGS += -Wno-old-style-definition

endif

endif

ifeq ($(CIRCUITPY_SDIOIO),1)
# Vendored Adafruit SdFat PIO SDIO card driver (C++). The common-hal C sources
# (SDCard.c, __init__.c) are picked up automatically through SRC_COMMON_HAL.
SRC_SDIOIO_CXX := \
	common-hal/sdioio/sdfat_pio/SdCard/PioSdio/PioSdioCard.cpp \
	common-hal/sdioio/sdfat_pio/shim.cpp \
	common-hal/sdioio/sdfat_pio/cxx_runtime.cpp \

INC += -Icommon-hal/sdioio/sdfat_pio

# The upstream driver body is guarded by ARDUINO_ARCH_RP2040 and was written
# against the Arduino RP2040 core, so define that macro for it. It is vendored
# third-party code, so don't fail the build on its warnings.
$(BUILD)/common-hal/sdioio/sdfat_pio/SdCard/PioSdio/PioSdioCard.o: CXXFLAGS += -DARDUINO_ARCH_RP2040 -Wno-error
endif

ifeq ($(CIRCUITPY_SSL),1)
# mbedtls 4.x keeps only TLS and X.509 in library/; crypto lives in the tf-psa-crypto
# submodule, split across core/, drivers/builtin/src/, extras/, platform/ and
# utilities/.
#
# Deliberately excluded:
#  - mbedtls_config.c, tf_psa_crypto_config.c: pure config-validation translation
#    units that #include generated *_config_check_*.h headers. Skipping them is what
#    lets us skip that second code generator.
#  - net_sockets.c: CircuitPython drives the socket through socketpool callbacks.
#  - pkcs7.c: unused, and MBEDTLS_PKCS7_C is off.
#  - psa_its_file.c: filesystem-backed PSA key storage; we have no persistent keys.
#  - drivers/{everest,p256-m,pqcp}: alternative curve and ML-DSA implementations.
mbedtls_sources = $(patsubst $(TOP)/%,%,$(filter-out $(2),$(wildcard $(TOP)/$(1)/*.c)))

SRC_MBEDTLS := \
	$(call mbedtls_sources,lib/mbedtls/library,\
		$(TOP)/lib/mbedtls/library/mbedtls_config.c \
		$(TOP)/lib/mbedtls/library/net_sockets.c \
		$(TOP)/lib/mbedtls/library/pkcs7.c) \
	$(call mbedtls_sources,lib/mbedtls/tf-psa-crypto/core,\
		$(TOP)/lib/mbedtls/tf-psa-crypto/core/psa_its_file.c \
		$(TOP)/lib/mbedtls/tf-psa-crypto/core/tf_psa_crypto_config.c) \
	$(call mbedtls_sources,lib/mbedtls/tf-psa-crypto/drivers/builtin/src) \
	$(call mbedtls_sources,lib/mbedtls/tf-psa-crypto/extras) \
	$(call mbedtls_sources,lib/mbedtls/tf-psa-crypto/platform) \
	$(call mbedtls_sources,lib/mbedtls/tf-psa-crypto/utilities)
SRC_C += $(SRC_MBEDTLS) lib/mbedtls_config/mbedtls_port.c lib/mbedtls_config/crt_bundle.c
# Public headers of mbedtls and of tf-psa-crypto, then the internal ones. mbedtls 4.x
# still reaches into tf-psa-crypto's internals, so upstream compiles the TLS layer
# with both; see TF_PSA_CRYPTO_LIBRARY_{PUBLIC,PRIVATE}_INCLUDE in
# lib/mbedtls/tf-psa-crypto/scripts/crypto-common.make.
CFLAGS += \
	  -isystem $(TOP)/lib/mbedtls/include \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/include \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/drivers/builtin/include \
	  -isystem $(TOP)/lib/mbedtls/library \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/core \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/dispatch \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/drivers/builtin/src \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/extras \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/platform \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/utilities \
	  -DMBEDTLS_CONFIG_FILE='"$(TOP)/lib/mbedtls_config/mbedtls_config.h"' \
	  -DTF_PSA_CRYPTO_CONFIG_FILE='"$(TOP)/lib/mbedtls_config/tf_psa_crypto_config.h"' \

# The generated bundle holds a hash of each root's subject name, not the name itself;
# the format is described in lib/mbedtls_config/crt_bundle.c.
$(BUILD)/x509_crt_bundle.S: $(TOP)/lib/certificates/data/roots-full.pem $(TOP)/tools/gen_crt_bundle.py
	$(Q)$(PYTHON) $(TOP)/tools/gen_crt_bundle.py -i $< -o $@ --asm
OBJ_MBEDTLS := $(BUILD)/x509_crt_bundle.o

# mbedtls 4.x dispatches PSA crypto through a wrapper layer that is generated from
# jinja templates rather than shipped in the repo, so build it.
# It is the same each time, so we could save it, but this makes updating mbedtls easier.
#
# generate_driver_wrappers.py resolves `mbedtls_framework` relative to its own tree,
# which would mean initializing a third nested submodule (tf-psa-crypto/framework).
# Point PYTHONPATH at the top-level framework submodule instead; mbedtls pins both to
# the same commit.
MBEDTLS_GEN := $(BUILD)/tf-psa-crypto
MBEDTLS_WRAPPERS_H := $(MBEDTLS_GEN)/psa_crypto_driver_wrappers.h
MBEDTLS_WRAPPERS_C := $(MBEDTLS_GEN)/psa_crypto_driver_wrappers_no_static.c

# The script writes both files at once. Naming both as targets of one rule would run
# it twice; see the same workaround in py/py.mk.
$(MBEDTLS_WRAPPERS_C): $(MBEDTLS_WRAPPERS_H)
	@true

$(MBEDTLS_WRAPPERS_H): $(TOP)/lib/mbedtls/tf-psa-crypto/scripts/generate_driver_wrappers.py $(MBEDTLS_DISPATCH_STAMP)
	$(STEPECHO) "GEN $@"
	$(Q)mkdir -p $(MBEDTLS_GEN)
	$(Q)PYTHONPATH=$(TOP)/lib/mbedtls/framework/scripts $(PYTHON) $< $(MBEDTLS_WRAPPERS_ARGS) $(MBEDTLS_GEN)

OBJ_MBEDTLS += $(MBEDTLS_WRAPPERS_C:.c=.o)
CFLAGS += -I$(MBEDTLS_GEN)

# psa_crypto.c and friends #include the generated header, so it has to exist before
# any of them compile.
$(patsubst %.c,$(BUILD)/%.o,$(SRC_MBEDTLS)) $(OBJ_MBEDTLS): $(MBEDTLS_WRAPPERS_H)
# Vendored third-party code, so don't hold it to CircuitPython's warning settings. The
# unused-but-set variables are real but harmless: they are in key-exchange paths that
# are compiled out. gcc 15.3 reports false array-bounds errors in the inlined
# mbedtls_xor_no_simd() in gcm.c and aes.c on RP2350.
$(patsubst %.c,$(BUILD)/%.o,$(SRC_MBEDTLS)) $(OBJ_MBEDTLS): CFLAGS += \
	-Wno-array-bounds \
	-Wno-suggest-attribute=format \
	-Wno-unused-but-set-variable
else
OBJ_MBEDTLS :=
endif

# SHA-256 on the RP2350 SHA-256 block, as a PSA driver that mbedtls's hash dispatch
# calls before its builtin code. The default and MBEDTLS_DRIVER_PATCH, which adds
# the driver to the dispatch, are set in mpconfigport.mk.
ifeq ($(CIRCUITPY_RP2_SHA256_HW),1)
SRC_C += sha256_psa/rp2_sha256_psa.c
CFLAGS += -Isha256_psa -isystem sdk/src/rp2_common/hardware_sha256/include/
endif
CFLAGS += -DCIRCUITPY_RP2_SHA256_HW=$(CIRCUITPY_RP2_SHA256_HW)

BOOT2_S_CFLAGS ?= -DPICO_FLASH_SPI_CLKDIV=4
SRC_S_UPPER = sdk/src/rp2_common/hardware_irq/irq_handler_chain.S \
              sdk/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S \
              sdk/src/rp2_common/pico_double/double_aeabi_$(DOUBLE_EABI).S \
              sdk/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S \
              sdk/src/rp2_common/pico_crt0/crt0.S \
              $(SRC_S_UPPER_CHIP_VARIANT)

ifeq ($(CIRCUITPY_PICODVI),1)
SRC_S_UPPER += lib/PicoDVI/software/libdvi/tmds_encode_asm.S \

endif

$(patsubst %.S,$(BUILD)/%.o,$(SRC_S_UPPER)): CFLAGS += -Wno-undef

# py/mkrules.mk derives CXXFLAGS from CFLAGS, which gives the vendored C++
# translation unit the SDK include paths and defines. Only the C++-specific
# options belong here.
CXXFLAGS += -std=gnu++17 -fno-exceptions -fno-rtti

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SDK:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S_UPPER:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))
OBJ += $(BUILD)/boot2_padded_checksummed.o
OBJ += $(OBJ_MBEDTLS)
ifeq ($(CIRCUITPY_SDIOIO),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_SDIOIO_CXX:.cpp=.o))
endif

$(BUILD)/%.o: $(BUILD)/%.S
	$(STEPECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/boot2_padded_checksummed.S: $(BUILD)/boot2.bin
	$(STEPECHO) "PAD_CHECKSUM $<"
	$(Q)$(PYTHON) sdk/src/rp2040/boot_stage2/pad_checksum -s 0xffffffff $< $@

$(BUILD)/boot2.bin: $(BUILD)/boot2.elf
	$(STEPECHO) "OBJCOPY $<"
	$(Q)$(OBJCOPY) -O binary $< $@


$(BUILD)/stage2.c: boot_stage2/$(CHIP_VARIANT).c.jinja gen_stage2.py | $(BUILD)/
	$(STEPECHO) "GEN $<"
	$(Q)$(PYTHON) gen_stage2.py $< $@ $(EXTERNAL_FLASH_DEVICES)

QSTR_GLOBAL_REQUIREMENTS += $(HEADER_BUILD)/flash_info.h
$(HEADER_BUILD)/flash_info.h: flash_info.h.jinja gen_stage2.py
	$(STEPECHO) "GEN $<"
	$(Q)$(PYTHON) gen_stage2.py $< $@ $(EXTERNAL_FLASH_DEVICES)

$(BUILD)/supervisor/internal_flash.o: $(HEADER_BUILD)/flash_info.h

$(BUILD)/boot2.elf: $(BUILD)/stage2.c
	$(STEPECHO) "BOOT $<"
	$(Q)$(CC) $(CFLAGS) $(BOOT2_S_CFLAGS) -Os -ggdb3 -I. -fPIC --specs=nosys.specs -nostartfiles -Wl,-T,boot_stage2/$(CHIP_VARIANT).ld  -Wl,-Map=$@.map -o $@ $<
	$(Q)$(SIZE) $@

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED) $(SRC_CIRCUITPY_COMMON)

all: $(BUILD)/firmware.uf2

BOARD_LD := $(wildcard boards/$(BOARD)/link.ld)

ifneq ($(BOARD_LD),)
	LINKER_SCRIPTS = -Wl,-T,$(BOARD_LD)
endif

LINKER_SCRIPTS += -Wl,-T,link-$(CHIP_VARIANT_LOWER).ld

# Builds where core1 locks flash out via the MPU and may only execute
# RAM-resident code: USB host (PIO-USB frame loop), and picodvi on RP2040
# (TMDS encode loop). Check the linked ELF from each core1 entry point.
# picodvi's DMA IRQ handler and scanline callback run on core1 but are
# reached only through registered pointers, so they are extra roots.
CORE1_CHECK_ROOTS :=
ifeq ($(CIRCUITPY_USB_HOST),1)
CORE1_CHECK_ROOTS += --root core1_main
endif
ifeq ($(CHIP_VARIANT),RP2040)
ifeq ($(CIRCUITPY_PICODVI),1)
CORE1_CHECK_ROOTS += --root core1_main --root dvi_dma1_irq --root core1_scanline_callback
endif
endif

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.elf: invalid-board
else
$(BUILD)/firmware.elf: $(OBJ) $(BOARD_LD) link-$(CHIP_VARIANT_LOWER).ld
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)echo $(PICO_LDFLAGS) > $(BUILD)/firmware.ldflags
	$(Q)$(CC) -o $@ $(CFLAGS) @$(BUILD)/firmware.ldflags $(LINKER_SCRIPTS) -Wl,--print-memory-usage -Wl,-Map=$@.map -Wl,-cref -Wl,--gc-sections @$(BUILD)/firmware.objs -Wl,-lc
	$(Q)$(SIZE) $@ | $(PYTHON) $(TOP)/tools/build_memory_info.py $@.map $(BUILD)
ifneq ($(CORE1_CHECK_ROOTS),)
	$(STEPECHO) "CHECK core1 flash calls"
	$(Q)$(PYTHON) $(TOP)/tools/check_core1_flash_calls.py $@ $(CORE1_CHECK_ROOTS)
endif
endif

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -R .dtcm_bss $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -f $(UF2_ID) -b 0x10000000 -c -o $@ $^

include $(TOP)/py/mkrules.mk
