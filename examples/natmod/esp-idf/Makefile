# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Espressif ESP-IDF path
IDF_PATH := /home/src/esp32/esp-idf-micropython
# Board to get correct ESP-IDF config
BOARD := GENERIC
# xtensa toolchain bin dir
PATH := $(IDF_PATH)/xtensa-esp32-elf/bin:$(PATH)

# Name of module
MOD = counter

# Source files (.c or .py)
SRC = counter.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH = xtensawin

# Include to get the rules for compiling and linking the module
include dyn_esp32.mk
include $(MPY_DIR)/py/dynruntime.mk

#plat_stubs.c: $(MPY_DIR)/ports/esp32/plat_relo.py
#	$(ECHO) "GEN $@"
#	$(Q)mkdir -p build
#	$(Q)$< -c >$@

build/plat_stubs.s: $(MPY_DIR)/ports/esp32/plat_relo.py
	$(ECHO) "GEN $@"
	$(Q)mkdir -p build
	$(Q)$< >$@
