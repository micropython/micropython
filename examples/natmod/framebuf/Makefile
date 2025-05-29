# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in framebuf so it can coexist)
MOD = framebuf_$(ARCH)

# Source files (.c or .py)
SRC = framebuf.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH ?= x64

# Link with libm.a and libgcc.a from the toolchain
LINK_RUNTIME = 1

ifeq ($(ARCH),xtensa)
EXTERN_SYMS=xtensa_externs.txt
endif

include $(MPY_DIR)/py/dynruntime.mk
