# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module
MOD = features2

# Source files (.c or .py)
SRC = main.c prod.c test.py

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc, rv64imc)
ARCH = x64

ifeq ($(findstring clang,$(shell $(CC) --version)),clang)
ifeq ($(ARCH),$(filter $(ARCH),armv6m armv7m rv32imc))
# Link with both libc.a and libclang_rt.builtins.a
LINK_CLANG_LIBC = 1
endif
endif

# Link with libm.a and libgcc.a from the toolchain
LINK_RUNTIME = 1

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk
