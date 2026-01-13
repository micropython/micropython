# Location of top-level MicroPython directory
MPY_DIR = ../../..

# Name of module
MOD = deepcraft_model

# Source files (.c or .py)
SRC = dc_mp_iface.c model.c

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin, rv32imc)
ARCH = armv7emsp

# Link with libm.a and libgcc.a from the toolchain
LINK_RUNTIME = 1

# OS-specific settings
ifeq ($(OS), Windows_NT)
    RM = del /Q /F
    CP = copy
    override MKDIR = cmd /C "if not exist $(1) mkdir $(1)"
    override PYTHON = python
    override ARCH_UPPER = $(shell powershell -Command "Write-Output '$(ARCH)'.ToUpper()")
    override MICROPY_FLOAT_IMPL_UPPER = $(shell powershell -Command "Write-Output '$(MICROPY_FLOAT_IMPL)'.ToUpper()")
    CLEAN_CMD = del /Q /F .mpy_ld_cache\* build\* $(MOD).mpy 2>nul
endif

CFLAGS += -Wno-error=implicit-function-declaration

override LIBGCC_PATH := gcc/lib/gcc/arm-none-eabi/11.3.1/thumb/v7e-m+fp/hard/libgcc.a
override LIBM_PATH := gcc/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libm.a

include $(MPY_DIR)/py/dynruntime.mk

$(BUILD_DIRS):
	$(Q)$(call MKDIR, $@)
