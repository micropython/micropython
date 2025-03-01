#
# Makefile fragment for STM32 MCUs.
#

# Create variables for the MCU name.
MCU_SERIES_UPPER = $(shell echo $(MCU_SERIES) | tr '[:lower:]' '[:upper:]')
CMSIS_MCU_LOWER = $(shell echo $(CMSIS_MCU) | tr '[:upper:]' '[:lower:]')

# Configure location of stm32lib.  Options are:
# 1. default: use provided lib/stm32lib
# 2. set STM32LIB_DIR to point to an stm32lib directory structure
# 3. set STM32LIB_CMSIS_DIR to point to xxx/CMSIS/Device/ST/STM32<series>xx
#    and STM32LIB_HAL_DIR to point to xxx/STM32<series>xx_HAL_Driver
ifneq ($(STM32LIB_CMSIS_DIR),)
# Option 3
STM32LIB_CMSIS_BASE = $(abspath $(STM32LIB_CMSIS_DIR))
STM32LIB_CMSIS_ABS = $(STM32LIB_CMSIS_BASE)
STM32LIB_HAL_BASE = $(abspath $(STM32LIB_HAL_DIR))
STM32LIB_HAL_ABS = $(STM32LIB_HAL_BASE)
else
ifneq ($(STM32LIB_DIR),)
# Option 2
STM32LIB_VPATH = $(abspath $(STM32LIB_DIR))
STM32LIB_FROM_HERE = $(STM32LIB_VPATH)
else
# Option 1
STM32LIB_VPATH = lib/stm32lib
STM32LIB_FROM_HERE = $(TOP)/$(STM32LIB_VPATH)
endif
STM32LIB_CMSIS_BASE = $(STM32LIB_VPATH)/CMSIS/STM32$(MCU_SERIES_UPPER)xx
STM32LIB_CMSIS_ABS = $(STM32LIB_FROM_HERE)/CMSIS/STM32$(MCU_SERIES_UPPER)xx
STM32LIB_HAL_BASE = $(STM32LIB_VPATH)/STM32$(MCU_SERIES_UPPER)xx_HAL_Driver
STM32LIB_HAL_ABS = $(STM32LIB_FROM_HERE)/STM32$(MCU_SERIES_UPPER)xx_HAL_Driver
endif

STARTUP_FILE ?= $(STM32LIB_CMSIS_BASE)/Source/Templates/gcc/startup_$(CMSIS_MCU_LOWER).o
SYSTEM_FILE ?= $(STM32LIB_CMSIS_BASE)/Source/Templates/system_stm32$(MCU_SERIES)xx.o

# Basic Cortex-M flags.
CFLAGS_CORTEX_M = -mthumb

ifneq ($(BUILDING_MBOOT),1)
# Select hardware floating-point support.
SUPPORTS_HARDWARE_FP_SINGLE = 0
SUPPORTS_HARDWARE_FP_DOUBLE = 0
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),STM32F765xx STM32F767xx STM32F769xx STM32H743xx STM32H747xx STM32H750xx STM32H7A3xx STM32H7A3xxQ STM32H7B3xx STM32H7B3xxQ))
CFLAGS_CORTEX_M += -mfpu=fpv5-d16 -mfloat-abi=hard -mfp16-format=ieee
SUPPORTS_HARDWARE_FP_SINGLE = 1
SUPPORTS_HARDWARE_FP_DOUBLE = 1
else
ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f0 g0 l0 l1 wl))
CFLAGS_CORTEX_M += -msoft-float
else
CFLAGS_CORTEX_M += -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mfp16-format=ieee
SUPPORTS_HARDWARE_FP_SINGLE = 1
endif
endif
endif

# Options for particular MCU series.
CFLAGS_MCU_f0 = $(CFLAGS_CORTEX_M) -mtune=cortex-m0 -mcpu=cortex-m0
CFLAGS_MCU_f4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_f7 = $(CFLAGS_CORTEX_M) -mtune=cortex-m7 -mcpu=cortex-m7
CFLAGS_MCU_g0 = $(CFLAGS_CORTEX_M) -mtune=cortex-m0plus -mcpu=cortex-m0plus
CFLAGS_MCU_g4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_l0 = $(CFLAGS_CORTEX_M) -mtune=cortex-m0plus -mcpu=cortex-m0plus
CFLAGS_MCU_l1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m3 -mcpu=cortex-m3
CFLAGS_MCU_l4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_h5 = $(CFLAGS_CORTEX_M) -mtune=cortex-m33 -mcpu=cortex-m33
CFLAGS_MCU_h7 = $(CFLAGS_CORTEX_M) -mtune=cortex-m7 -mcpu=cortex-m7
CFLAGS_MCU_wb = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_wl = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4

MPY_CROSS_MCU_ARCH_f0 = armv6m
MPY_CROSS_MCU_ARCH_f4 = armv7m
MPY_CROSS_MCU_ARCH_f7 = armv7m
MPY_CROSS_MCU_ARCH_g0 = armv6m
MPY_CROSS_MCU_ARCH_g4 = armv7m
MPY_CROSS_MCU_ARCH_l0 = armv6m
MPY_CROSS_MCU_ARCH_l1 = armv7m
MPY_CROSS_MCU_ARCH_l4 = armv7m
MPY_CROSS_MCU_ARCH_h5 = armv7m
MPY_CROSS_MCU_ARCH_h7 = armv7m
MPY_CROSS_MCU_ARCH_wb = armv7m
MPY_CROSS_MCU_ARCH_wl = armv7m
