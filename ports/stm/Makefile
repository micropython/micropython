# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2019 Dan Halbert for Adafruit Industries
# Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
# DEBUG = 1

# Select the board to build for.
ifeq ($(BOARD),)
	$(error You must provide a BOARD parameter)
else
	ifeq ($(wildcard boards/$(BOARD)/.),)
		$(error Invalid BOARD specified)
	endif
endif

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

CROSS_COMPILE = arm-none-eabi-

#######################################
# CFLAGS
#######################################

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I./st_driver/STM32F4xx_HAL_Driver/Inc
INC += -I./st_driver/STM32F4xx_HAL_Driver/Inc/Legacy
INC += -I./st_driver/CMSIS/Device/ST/STM32F4xx/Include
INC += -I./st_driver/CMSIS/Include
INC += -I./boards
INC += -I./boards/$(BOARD)
INC += -I./peripherals
INC += -I../../lib/mp-readline
INC += -I../../lib/tinyusb/src
INC += -I../../supervisor/shared/usb


#Debugging/Optimization
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb
	# You may want to enable these flags to make setting breakpoints easier.
	CFLAGS += -fno-inline -fno-ipa-sra
else
	CFLAGS += -Os -DNDEBUG
	CFLAGS += -ggdb
	# TODO: Test with -flto
	### CFLAGS += -flto
endif


C_DEFS = -DMCU_PACKAGE=$(MCU_PACKAGE) -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -D$(CMSIS_MCU)

CFLAGS += $(INC) -Werror -Wall -std=gnu11 -nostdlib $(BASE_CFLAGS) $(C_DEFS) $(CFLAGS_MOD) $(COPT)

# Undo some warnings.
# STM32 apparently also uses undefined preprocessor variables quite casually,
# so we can't do warning checks for these.
CFLAGS += -Wno-undef
# STM32 might do casts that increase alignment requirements.
CFLAGS += -Wno-cast-align

CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mfloat-abi=hard \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16

# TODO: check this
CFLAGS += -D__START=main

#need both command and valid file to use uf2 bootloader
ifndef LD_FILE
    ifneq ($(and $(UF2_BOOTLOADER),$(LD_BOOT)),)
        LD_FILE = $(LD_BOOT)
        BOOTLOADER_OFFSET = $(UF2_OFFSET)
        CFLAGS += -DUF2_BOOTLOADER_ENABLED
    else
        LD_FILE = $(LD_DEFAULT)
    endif
endif

# Add bootloader specific items
ifndef BOOTLOADER_OFFSET
	BOOTLOADER_OFFSET := 0x8000000
endif

LDFLAGS = $(CFLAGS) -fshort-enums -Wl,-nostdlib -Wl,-T,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc

LDFLAGS += -mthumb -mcpu=cortex-m4

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_STM32F4 -DCFG_TUD_CDC_RX_BUFSIZE=1024 -DCFG_TUD_CDC_TX_BUFSIZE=1024 -DCFG_TUD_MSC_BUFSIZE=4096 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_MIDI_TX_BUFSIZE=128


######################################
# source
######################################

SRC_STM32 = \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_qspi.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_i2c.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usart.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c \
	st_driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c \
	system_stm32f4xx.c


SRC_C += \
	background.c \
	fatfs_port.c \
	mphalport.c \
	tick.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	peripherals/stm32f4/$(MCU_SUB_VARIANT)/pins.c \
	peripherals/stm32f4/$(MCU_SUB_VARIANT)/clocks.c \
	peripherals/stm32f4/$(MCU_SUB_VARIANT)/gpio.c \
	peripherals/stm32f4/$(MCU_SUB_VARIANT)/periph.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	supervisor/shared/memory.c

ifneq ($(USB),FALSE)
SRC_C += lib/tinyusb/src/portable/st/synopsys/dcd_synopsys.c
endif

SRC_S = \
	supervisor/cpu.s \
	boards/startup_$(MCU_SUB_VARIANT).s

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
OBJ += $(addprefix $(BUILD)/, $(SRC_STM32:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR +=


all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON3) $(TOP)/tools/build_memory_info.py $(LD_FILE)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@
#   $(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $^ $@
#   $(Q)$(OBJCOPY) -O ihex -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.hex
	$(ECHO) "Create $@"
	$(PYTHON3) $(TOP)/tools/uf2/utils/uf2conv.py -f 0x57755a57 -b $(BOOTLOADER_OFFSET) -c -o "$(BUILD)/firmware.uf2" $^

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
