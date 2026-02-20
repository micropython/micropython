################################################################################
# Initial setup of Makefile environment

BOARD ?= KIT_PSE84_AI
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Port and board specific make settings.
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# qstr definitions (must come before including py.mk).
QSTR_DEFS += qstrdefsport.h

# Include py core make definitions.
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags

# Define toolchain and other tools.
CROSS_COMPILE ?= arm-none-eabi-
EDGEPROTECTTOOLS ?= edgeprotecttools
OPENOCD ?= /opt/Tools/ModusToolboxProgtools-1.5/openocd/bin/openocd

# SDK locations.
CMSIS_DIR ?= $(TOP)/lib/cmsis/inc
PSOC_LIB_REL_TOP ?= lib/psoc-edge
PSOC_LIB_REL_HERE ?= $(TOP)/$(PSOC_LIB_REL_TOP)

INC += -I.
INC += -Ibsp
INC += -Imcu
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -I$(CMSIS_DIR)
INC += -I$(PSOC_LIB_REL_HERE)/TARGET_KIT_PSE84_AI
INC += -I$(PSOC_LIB_REL_HERE)/TARGET_KIT_PSE84_AI/COMPONENT_CM33/COMPONENT_SECURE_DEVICE
INC += -I$(PSOC_LIB_REL_HERE)/core-lib/include
INC += -I$(PSOC_LIB_REL_HERE)/mtb-dsl-pse8xxgp/pdl/drivers/include
INC += -I$(PSOC_LIB_REL_HERE)/mtb-dsl-pse8xxgp/pdl/devices/include
INC += -I$(PSOC_LIB_REL_HERE)/mtb-dsl-pse8xxgp/pdl/devices/include/ip
INC += -I$(PSOC_LIB_REL_HERE)/mtb-dsl-pse8xxgp/hal/include
INC += -I$(PSOC_LIB_REL_HERE)/mtb-srf/include
INC += -I$(PSOC_LIB_REL_HERE)/se-rt-services-utils

# Set CFLAGS.
CFLAGS += $(INC)
CFLAGS += -Wall -Werror -std=c99
CFLAGS += -mthumb -mtune=cortex-m33 -mcpu=cortex-m33 -mcmse
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mfp16-format=ieee
CFLAGS += --specs=nosys.specs
CFLAGS += -DCORE_NAME_CM33_0
CFLAGS += -DCOMPONENT_CM33
CFLAGS += -DCOMPONENT_SECURE_DEVICE
CFLAGS += -DCOMPONENT_MTB_HAL
CFLAGS += -DCY_PDL_ENABLE_SECURE_AWARE_SYSCLK=0

# Select debugging or optimisation build.
ifeq ($(DEBUG), 1)
CFLAGS += -Og
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Set assembler flags.
AFLAGS = -mthumb -march=armv8.1-m.main+fp+mve.fp

# Set linker flags (using the compiler to get the stdlib).
CFLAGS += \
	-Wl,-Tmcu/pse84.ld \
	-Wl,--gc-sections \
	-Wl,--no-warn-rwx-segment \

################################################################################
# Source files and libraries

SRC_O += \
	shared/runtime/gchelper_thumb2.o \
	$(PSOC_LIB_REL_TOP)/mtb-dsl-pse8xxgp/pdl/drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.o \

# Define the required source files.
SRC_C += \
	main.c \
	modpsocedge.c \
	mphalport.c \
	uart_stdio.c \
	$(wildcard $(BOARD_DIR)/*.c)

SHARED_SRC_C += $(addprefix shared/,\
	readline/readline.c \
	runtime/gchelper_native.c \
	runtime/interrupt_char.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	)

ifeq ($(MICROPY_FLOAT_IMPL),float)
LIBM_SRC_C += $(SRC_LIB_LIBM_C)
LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_HW_C)
$(BUILD)/lib/libm/%.o: CFLAGS += -Wno-maybe-uninitialized
endif

BSP_SRC_C += \
	bsp/cycfg.c \
	bsp/cycfg_clocks.c \
	bsp/cycfg_peripheral_clocks.c \
	bsp/cycfg_peripherals.c \
	bsp/cycfg_system.c \
	bsp/cycfg_routing.c \
	bsp/cycfg_protection.c \
	bsp/cycfg_pins.c \

PSOC_LIB_SRC_C += $(addprefix $(PSOC_LIB_REL_TOP)/,\
	TARGET_KIT_PSE84_AI/COMPONENT_CM33/COMPONENT_SECURE_DEVICE/s_start_pse84.c \
	TARGET_KIT_PSE84_AI/COMPONENT_CM33/COMPONENT_SECURE_DEVICE/s_system_pse84.c \
	TARGET_KIT_PSE84_AI/cybsp.c \
	TARGET_KIT_PSE84_AI/system_edge.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_clock.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_gpio.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_system.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_timer.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_uart.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_utils_impl.c \
	mtb-dsl-pse8xxgp/pdl/devices/source/cy_device.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_gpio.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_mpc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ppc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_rtc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_common.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_uart.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysclk_v2.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syslib.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_pdcm.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_ppu.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_v4.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_tcpwm_counter.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_trigmux.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_wdt.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysint_v2.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/ppu_v1.c \
	se-rt-services-utils/ifx_se_crc32.c \
	se-rt-services-utils/ifx_se_fih.c \
	se-rt-services-utils/ifx_se_platform.c \
	se-rt-services-utils/ifx_se_syscall.c \
	se-rt-services-utils/ifx_se_syscall_builtin.c \
	)

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C)

# Define the required object files.
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(BSP_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSOC_LIB_SRC_C:.c=.o))

################################################################################
# Main targets

# Define the top-level target, the main firmware.
all: $(BUILD)/firmware.signed.hex

$(BUILD)/firmware.elf: $(OBJ) mcu/pse84.ld
	$(ECHO) "Link $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $(OBJ)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $^ $@

$(BUILD)/firmware.signed.hex: $(BUILD)/firmware.hex
	$(Q)$(EDGEPROTECTTOOLS) image-metadata --image $^ --output $@ --erased-val 0xff --hex-addr 0x70100000 --header-size 0x400 --slot-size 0x80000

deploy: $(BUILD)/firmware.signed.hex
	$(Q)$(OPENOCD) -s mcu -f mcu/openocd.cfg '-c init' -c 'reset init' -c 'flash write_image erase $^' -c 'reset run' -c shutdown

objdump:
	arm-none-eabi-objdump -xD $(BUILD)/firmware.elf | less

gdb-server:
	$(Q)$(OPENOCD) -s mcu -f mcu/openocd.cfg

################################################################################
# Remaining make rules

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
