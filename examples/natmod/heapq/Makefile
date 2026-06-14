# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module (different to built-in heapq so it can coexist)
MOD_BASE = heapq
MOD = $(MOD_BASE)_$(ARCH)

# Source files (.c or .py)
SRC = heapq.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc, rv64imc)
ARCH = x64

# Strip the architecture name from the internal filename.
MPY_LD_FLAGS = "--source-name=$(MOD_BASE).mpy"

include $(MPY_DIR)/py/dynruntime.mk
