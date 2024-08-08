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
	-isystem lib/cyw43-driver/firmware \
	-isystem lib/cyw43-driver/src \
	-isystem lib/lwip/src/include \
	-isystem sdk/src/rp2_common/pico_async_context/include/ \
	-isystem sdk/src/rp2_common/pico_cyw43_arch/include/ \
	-isystem sdk/src/rp2_common/pico_cyw43_driver/include/ \
	-isystem sdk/src/rp2_common/pico_lwip/include/ \
	-isystem sdk/src/rp2_common/pico_rand/include/ \

CFLAGS_CYW43 := -DCYW43_LWIP=1 -DPICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1 -DCYW43_USE_SPI -DIGNORE_GPIO25 -DIGNORE_GPIO23 -DIGNORE_GPIO24 -DCYW43_LOGIC_DEBUG=0 -DCYW43_USE_STATS=0 -DPICO_BUILD -DCYW43_ENABLE_BLUETOOTH=0 -DPICO_CYW43_ARCH_POLL=0
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

CHIP_VARIANT_LOWER = $(shell echo $(CHIP_VARIANT) | tr '[:upper:]' '[:lower:]')

INC += \
	-I. \
	-Ilwip_inc \
	-I../.. \
	-I../lib/mp-readline \
	-I../shared/timeutils \
	-Iboards/$(BOARD) \
	-Iboards/ \
	-isystem ./../../lib/cmsis/inc \
	-isystem sdk/ \
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
	-isystem sdk/src/rp2_common/pico_aon_timer/include/ \
	-isystem sdk/src/rp2_common/pico_atomic/include/ \
	-isystem sdk/src/rp2_common/pico_bootrom/include/ \
	-isystem sdk/src/rp2_common/pico_double/include/ \
	-isystem sdk/src/rp2_common/pico_multicore/include/ \
	-isystem sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include/ \
	-isystem sdk/src/rp2_common/pico_stdio/include/ \
	-isystem sdk/src/rp2_common/pico_printf/include/ \
	-isystem sdk/src/rp2_common/pico_float/include/ \
	-isystem sdk/src/rp2_common/pico_runtime/include/ \
	-isystem sdk/src/rp2_common/pico_runtime_init/include/ \
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
	-DPICO_ON_DEVICE=1 \
	-DPICO_NO_BINARY_INFO=0 \
	-DPICO_TIME_DEFAULT_ALARM_POOL_DISABLED=0 \
	-DPICO_DIVIDER_CALL_IDIV0=0 \
	-DPICO_DIVIDER_CALL_LDIV0=0 \
	-DPICO_DIVIDER_HARDWARE=1 \
	-DPICO_DOUBLE_ROM=1 \
	-DPICO_FLOAT_ROM=1 \
	-DLIB_PICO_MULTICORE=1 \
	-DPICO_BITS_IN_RAM=0 \
	-DPICO_DIVIDER_IN_RAM=0 \
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

OPTIMIZATION_FLAGS ?= -O3
# TinyUSB defines
CFLAGS += \
	-DCFG_TUSB_OS=OPT_OS_PICO \
	-DTUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1 \
	-DCFG_TUSB_MCU=OPT_MCU_RP2040 \
	-DCFG_TUD_MIDI_RX_BUFSIZE=128 \
	-DCFG_TUD_CDC_RX_BUFSIZE=256 \
	-DCFG_TUD_MIDI_TX_BUFSIZE=128 \
	-DCFG_TUD_CDC_TX_BUFSIZE=256 \
	-DCFG_TUD_MSC_BUFSIZE=1024

# option to override default optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

# flags specific to wifi / cyw43

CFLAGS += $(CFLAGS_CYW43)
#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -ggdb3 -O3
  # No LTO because we may place some functions in RAM instead of flash.
else
  CFLAGS += -DNDEBUG

  # No LTO because we may place some functions in RAM instead of flash.

  ifdef CFLAGS_BOARD
    CFLAGS += $(CFLAGS_BOARD)
  endif
endif

# Remove -Wno-stringop-overflow after we can test with CI's GCC 10. Mac's looks weird.
DISABLE_WARNINGS = -Wno-stringop-overflow -Wno-cast-align

CFLAGS += $(INC) -Wall -Werror -std=gnu11 -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) $(DISABLE_WARNINGS) -Werror=missing-prototypes

PICO_LDFLAGS = --specs=nosys.specs --specs=nano.specs

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

LIBS += -lc

ifeq ($(CHIP_VARIANT),RP2040)
CFLAGS += \
	-march=armv6-m \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft

CFLAGS += \
	-DPICO_RP2040

SRC_SDK_CHIP_VARIANT := \
	src/rp2_common/hardware_rtc/rtc.c \
	src/rp2_common/pico_double/double_init_rom_rp2040.c \
	src/rp2_common/pico_float/float_init_rom_rp2040.c \
	src/rp2_common/pico_float/float_math.c \

SRC_S_UPPER_CHIP_VARIANT := \
	sdk/src/rp2_common/hardware_divider/divider.S \
	sdk/src/rp2_common/pico_divider/divider_hardware.S \
	sdk/src/rp2_common/pico_double/double_v1_rom_shim_rp2040.S \
	sdk/src/rp2_common/pico_float/float_v1_rom_shim_rp2040.S \
	sdk/src/rp2_common/pico_float/float_aeabi_rp2040.S \
	sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S \

# Wrap a bunch of math stuff to use the Pico SDK divider
PICO_LDFLAGS += -Wl,--wrap=__aeabi_ldiv0 \
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
								-Wl,--wrap=__aeabi_uldivmod \
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
								-Wl,--wrap=fma \
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
								-Wl,--wrap=sqrtf \
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
								-Wl,--wrap=fmaf \
								-Wl,--wrap=memcpy \
								-Wl,--wrap=memset \
								-Wl,--wrap=__aeabi_memcpy \
								-Wl,--wrap=__aeabi_memset \
								-Wl,--wrap=__aeabi_memcpy4 \
								-Wl,--wrap=__aeabi_memset4 \
								-Wl,--wrap=__aeabi_memcpy8 \
								-Wl,--wrap=__aeabi_memset8

UF2_ID = 0xE48BFF56

DOUBLE_EABI = rp2040
endif
ifeq ($(CHIP_VARIANT),RP2350)
CFLAGS += \
	-march=armv8-m.main+fp+dsp \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m33 \
	-mfloat-abi=softfp

# ARM Secure family id
UF2_ID = 0xe48bff59

# Double coprocessor is only available on the ARM core.
DOUBLE_EABI = dcp
INC += \
	-isystem sdk/src/rp2_common/hardware_dcp/include/

CFLAGS += -DPICO_RP2350=1

SRC_SDK_CHIP_VARIANT := \
	src/rp2_common/hardware_powman/powman.c \

ifeq ($(CHIP_PACKAGE),A)
CFLAGS += -DPICO_RP2350A=1
CFLAGS += -DPICO_RP2350B=0
else
CFLAGS += -DPICO_RP2350A=0
CFLAGS += -DPICO_RP2350B=1
endif

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
	src/rp2_common/hardware_xosc/xosc.c \
	src/rp2_common/pico_aon_timer/aon_timer.c \
	src/rp2_common/pico_atomic/atomic.c \
	src/rp2_common/pico_bootrom/bootrom.c \
	src/rp2_common/pico_bootsel_via_double_reset/pico_bootsel_via_double_reset.c \
	src/rp2_common/pico_clib_interface/newlib_interface.c \
	src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c \
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
	bindings/rp2pio/StateMachine.c \
	bindings/rp2pio/__init__.c \
	common-hal/rp2pio/StateMachine.c \
	common-hal/rp2pio/__init__.c \
	audio_dma.c \
	background.c \
	peripherals/pins.c \
	lib/crypto-algorithms/sha256.c \
	lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c \
	lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c \
	mphalport.c \
	$(SRC_CYW43) \
	$(SRC_LWIP) \


ifeq ($(CIRCUITPY_USB_HOST), 1)
SRC_C += \
	lib/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c \
	lib/Pico-PIO-USB/src/pio_usb.c \
	lib/Pico-PIO-USB/src/pio_usb_host.c \
	lib/Pico-PIO-USB/src/usb_crc.c \

INC += \
	-isystem lib/Pico-PIO-USB/src
endif

ifeq ($(CIRCUITPY_PICODVI),1)
SRC_C += \
	bindings/picodvi/__init__.c \
	bindings/picodvi/Framebuffer.c \
	common-hal/picodvi/Framebuffer_$(CHIP_VARIANT).c \

ifeq ($(CHIP_VARIANT),RP2040)
SRC_C += \
	lib/PicoDVI/software/libdvi/dvi.c \
	lib/PicoDVI/software/libdvi/dvi_serialiser.c \
	lib/PicoDVI/software/libdvi/dvi_timing.c \
	lib/PicoDVI/software/libdvi/tmds_encode.c \

endif

endif

ifeq ($(CIRCUITPY_SSL),1)
CFLAGS += -isystem $(TOP)/mbedtls/include
SRC_MBEDTLS := $(addprefix lib/mbedtls/library/, \
	aes.c \
	aesni.c \
	arc4.c \
	asn1parse.c \
	asn1write.c \
	base64.c \
	bignum.c \
	blowfish.c \
	camellia.c \
	ccm.c \
	certs.c \
	chacha20.c \
	chachapoly.c \
	cipher.c \
	cipher_wrap.c \
	cmac.c \
	constant_time.c \
	ctr_drbg.c \
	debug.c \
	des.c \
	dhm.c \
	ecdh.c \
	ecdsa.c \
	ecjpake.c \
	ecp.c \
	ecp_curves.c \
	entropy.c \
	entropy_poll.c \
	gcm.c \
	havege.c \
	hmac_drbg.c \
	md2.c \
	md4.c \
	md5.c \
	md.c \
	oid.c \
	padlock.c \
	pem.c \
	pk.c \
	pkcs11.c \
	pkcs12.c \
	pkcs5.c \
	pkparse.c \
	pk_wrap.c \
	pkwrite.c \
	platform.c \
	platform_util.c \
	poly1305.c \
	ripemd160.c \
	rsa.c \
	rsa_internal.c \
	sha1.c \
	sha256.c \
	sha512.c \
	ssl_cache.c \
	ssl_ciphersuites.c \
	ssl_cli.c \
	ssl_cookie.c \
	ssl_msg.c \
	ssl_srv.c \
	ssl_ticket.c \
	ssl_tls.c \
	timing.c \
	x509.c \
	x509_create.c \
	x509_crl.c \
	x509_crt.c \
	x509_csr.c \
	x509write_crt.c \
	x509write_csr.c \
	xtea.c \
	)
SRC_C += $(SRC_MBEDTLS) lib/mbedtls_config/mbedtls_port.c lib/mbedtls_config/crt_bundle.c
CFLAGS += \
	  -isystem $(TOP)/lib/mbedtls/include \
	  -DMBEDTLS_CONFIG_FILE='"$(TOP)/lib/mbedtls_config/mbedtls_config.h"' \

$(BUILD)/x509_crt_bundle.S: $(TOP)/lib/certificates/data/roots.pem $(TOP)/tools/gen_crt_bundle.py
	$(Q)$(PYTHON) $(TOP)/tools/gen_crt_bundle.py -i $< -o $@ --asm
OBJ_MBEDTLS := $(BUILD)/x509_crt_bundle.o
$(patsubst %.c,$(BUILD)/%.o,$(SRC_MBEDTLS))): CFLAGS += -Wno-suggest-attribute=format
else
OBJ_MBEDTLS :=
endif


SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# There may be duplicates between SRC_COMMON_HAL_EXPANDED and SRC_SHARED_MODULE_EXPANDED,
# because a few modules have files both in common-hal/ and shared-module/.
# Doing a $(sort ...) removes duplicates as part of sorting.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))

SRC_S = supervisor/$(CHIP_FAMILY)_cpu.s
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

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED) $(SRC_CIRCUITPY_COMMON)

all: $(BUILD)/firmware.uf2

BOARD_LD := $(wildcard boards/$(BOARD)/link.ld)

ifneq ($(BOARD_LD),)
	LINKER_SCRIPTS = -Wl,-T,$(BOARD_LD)
endif

LINKER_SCRIPTS += -Wl,-T,link-$(CHIP_VARIANT_LOWER).ld

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.elf: invalid-board
else
$(BUILD)/firmware.elf: $(OBJ) $(BOARD_LD) link-$(CHIP_VARIANT_LOWER).ld
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)echo $(PICO_LDFLAGS) > $(BUILD)/firmware.ldflags
	$(Q)$(CC) -o $@ $(CFLAGS) @$(BUILD)/firmware.ldflags $(LINKER_SCRIPTS) -Wl,--print-memory-usage -Wl,-Map=$@.map -Wl,-cref -Wl,--gc-sections @$(BUILD)/firmware.objs -Wl,-lc
endif

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -R .dtcm_bss $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -f $(UF2_ID) -b 0x10000000 -c -o $@ $^

include $(TOP)/py/mkrules.mk
