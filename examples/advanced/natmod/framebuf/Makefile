# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in framebuf so it can coexist)
MOD = framebuf_$(ARCH)

# Source files (.c or .py)
SRC = framebuf.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH ?= x64

ifeq ($(ARCH),armv6m)
# Link with libgcc.a for division helper functions
LINK_RUNTIME = 1
endif

ifeq ($(ARCH),xtensa)
MPY_EXTERN_SYM_FILE=$(MPY_DIR)/ports/esp8266/boards/eagle.rom.addr.v6.ld
endif

include $(MPY_DIR)/py/dynruntime.mk
