# Location of top-level MicroPython directory
MPY_DIR = $(abspath ../../..)

# Name of module
MOD = esp32heap

# Source files (.c or .py)
SRC = features0.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH = xtensawin

PORT = esp32

# Location of MicroPython source tree
TOP := $(abspath ../../..)

# Espressif ESP-IDF path
IDF_PATH ?= $(abspath $(TOP)/../esp-idf-micropython)

# xtensa toolchain bin dir
PATH := $(abspath $(TOP)/../xtensa-esp32-elf-8.2.0/bin):$(PATH)

# Board to get correct ESP-IDF config
BOARD ?= GENERIC

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk
include $(MPY_DIR)/ports/$(PORT)/dynruntime.mk
