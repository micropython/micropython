include ../py/mkenv.mk

# define main target
PROG = mpy-cross

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I$(BUILD)
INC += -I$(TOP)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wextra -Wno-unused-parameter -Wpointer-arith
CFLAGS += $(INC) $(CWARN) -std=gnu99 $(COPT) $(CFLAGS_EXTRA)
CFLAGS += -fdata-sections -ffunction-sections -fno-asynchronous-unwind-tables

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
LDFLAGS += $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

# source files
SRC_C = \
	main.c \
	gccollect.c \
	shared/runtime/gchelper_generic.c \

# Workaround for building native mpy-cross when invoked inside an Espressif
# Installation Manager (EIM) environment (ie. ESP32 builds).
#
# EIM prepends an LLVM installation in front of the system PATH (so macOS can't
# compile native binaries) and adds directories to the PATH variable whose files
# collide with an existing GCC installation as they recursively add any
# directory named `bin` under the tools root of the currently chosen ESP-IDF
# environment.  Of course nobody would ever build native binaries to do any pre
# or post build actions or anything else for an ESP32 project, right?

ifneq ($(IDF_TOOLS_PATH),)
# This may break if one or more path entries contain spaces in their name and
# the compiler is under one of those directories.
SPACE := $(subst ,, )
PATH_COPY := $(PATH)
# Filter out all directories under IDF_TOOLS_PATH from the system PATH.
PATH = $(subst $(SPACE),:,$(filter-out $(IDF_TOOLS_PATH)/%,$(subst :,$(SPACE),$(PATH_COPY))))
endif

# Add fmode when compiling with mingw gcc
COMPILER_TARGET := $(shell $(CC) -dumpmachine)
ifneq (,$(findstring mingw,$(COMPILER_TARGET)))
	SRC_C += ports/windows/fmode.c
endif

OBJ = $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

include $(TOP)/py/mkrules.mk
