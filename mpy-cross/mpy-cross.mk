# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

include ../py/mkenv.mk

# define main target

ifeq ($(OS),Windows_NT)
# Detect a MINGW32 build, and change the name of the final executable.
PROG ?= mpy-cross.exe
else
PROG ?= mpy-cross
endif

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -std=gnu99 $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)
CFLAGS += -fdata-sections -ffunction-sections -fno-asynchronous-unwind-tables

# Build a static executable.
# Useful for Windows builds, etc., that must run on multiple operating system versions.
ifdef STATIC_BUILD
CFLAGS += -static -static-libgcc -static-libstdc++
endif


# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of MicroPython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
CC = clang
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map -Wl,-dead_strip
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif
LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

ifdef STATIC_BUILD
LDFLAGS += -static -static-libgcc -static-libstdc++
endif

# source files
SRC_C += \
	main.c \
	gccollect.c \
	lib/utils/gchelper_generic.c \
	supervisor/stub/safe_mode.c \
	supervisor/stub/stack.c \
	supervisor/shared/translate.c

# Add fmode when compiling with mingw gcc
COMPILER_TARGET := $(shell $(CC) -dumpmachine)
ifneq (,$(findstring mingw,$(COMPILER_TARGET)))
	SRC_C += fmode.c
endif

OBJ = $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

$(BUILD)/supervisor/shared/translate.o: $(HEADER_BUILD)/qstrdefs.generated.h

include $(TOP)/py/mkrules.mk
