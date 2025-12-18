# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in uzlib so it can coexist)
MOD = deflate_$(ARCH)

# Source files (.c or .py)
SRC = deflate.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc)
ARCH ?= x64

ifeq ($(ARCH),armv6m)
# Link with libgcc.a for division helper functions
LINK_RUNTIME = 1
endif

ifeq ($(ARCH),xtensa)
# Link with libm.a and libgcc.a from the toolchain
LINK_RUNTIME = 1
MPY_EXTERN_SYM_FILE=$(MPY_DIR)/ports/esp8266/boards/eagle.rom.addr.v6.ld
endif

include $(MPY_DIR)/py/dynruntime.mk
