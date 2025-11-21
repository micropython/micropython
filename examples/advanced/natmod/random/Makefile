# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in random so it can coexist)
MOD = random_$(ARCH)

# Source files (.c or .py)
SRC = random.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc)
ARCH ?= x64

ifeq ($(ARCH),xtensa)
MPY_EXTERN_SYM_FILE=$(MPY_DIR)/ports/esp8266/boards/eagle.rom.addr.v6.ld
endif

ifeq ($(ARCH),$(filter $(ARCH),armv6m armv7m))
# Link with libm.a for soft-float helper functions
LINK_RUNTIME = 1
endif

include $(MPY_DIR)/py/dynruntime.mk
