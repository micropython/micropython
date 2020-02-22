# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Espressif ESP-IDF path
IDF_PATH := /home/src/esp32/esp-idf-micropython
# Board to get correct ESP-IDF config
BOARD := GENERIC
# xtensa toolchain bin dir
PATH := $(IDF_PATH)/xtensa-esp32-elf/bin:$(PATH)

# Name of module
MOD = espidf

# Source files (.c or .py)
SRC = modespidf.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH = xtensawin

# Include to get the rules for compiling and linking the module
include dyn_esp32.mk
include $(MPY_DIR)/py/dynruntime.mk
CFLAGS += -std=gnu99 # override -std=c99 in dynruntime.mk
CFLAGS += -DMPY_DYN_MODULE=1

modespidf.c: $(MPY_DIR)/ports/esp32/plat_relo.py build/sdkconfig.h
	$(ECHO) "GEN $@"
	$(Q)mkdir -p build
	$(Q)$< --module >$@

build/sdkconfig.h: $(BOARD_BUILD)/sdkconfig.h build
	$(Q)cp $< $@

build:
	$(Q)mkdir -p $@
