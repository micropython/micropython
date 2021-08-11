MCU_SERIES = f4
CMSIS_MCU = STM32F413xx
AF_FILE = boards/stm32f413_af.csv
LD_FILES = boards/LEGO_HUB_NO6/stm32f413xg.ld boards/common_bl.ld
TEXT0_ADDR = 0x08010000

BOOTLOADER_DFU_USB_VID ?= 0x0694
BOOTLOADER_DFU_USB_PID ?= 0x0008

# MicroPython settings
MICROPY_PY_BLUETOOTH ?= 1
MICROPY_BLUETOOTH_NIMBLE ?= 0
MICROPY_BLUETOOTH_BTSTACK ?= 1
MICROPY_VFS_LFS2 ?= 1

ifneq ($(BUILDING_MBOOT),1)
LIB_SRC_C += lib/btstack/chipset/cc256x/btstack_chipset_cc256x.c
endif

# Bootloader settings
MBOOT_TEXT0_ADDR = 0x08008000
MBOOT_LD_FILES = ../boards/LEGO_HUB_NO6/mboot_memory.ld stm32_sections.ld
