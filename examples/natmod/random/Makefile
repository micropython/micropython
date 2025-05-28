# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in random so it can coexist)
MOD = random_$(ARCH)

# Source files (.c or .py)
SRC = random.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc)
ARCH = x64

ifeq ($(ARCH),xtensa)
# Link with libm.a and libgcc.a from the toolchain
LINK_RUNTIME = 1
endif

include $(MPY_DIR)/py/dynruntime.mk
