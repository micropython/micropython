# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in re so it can coexist)
MOD = re_$(ARCH)

# Source files (.c or .py)
SRC = re.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc)
ARCH = x64

ifeq ($(ARCH),armv6m)
# Link with libgcc.a for division helper functions
LINK_RUNTIME = 1
endif

include $(MPY_DIR)/py/dynruntime.mk
