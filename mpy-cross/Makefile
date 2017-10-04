# The following is a temporary hack to forefully undefine vars that might have
# be defined by a calling Makefile (from recursive make).
# TODO: Find a better way to be able to call this Makefile recursively.
ifneq ($(findstring undefine,$(.FEATURES)),)
override undefine COPT
override undefine CFLAGS_EXTRA
override undefine LDFLAGS_EXTRA
override undefine FROZEN_DIR
override undefine FROZEN_MPY_DIR
override undefine BUILD
override undefine PROG
endif

include ../py/mkenv.mk

# define main target
PROG = mpy-cross

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk

INC +=  -I.
INC +=  -I$(TOP)
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -std=gnu99 $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)
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
LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

# source files
SRC_C = \
	main.c \
	gccollect.c \

# Add fmode when compiling with mingw gcc
COMPILER_TARGET := $(shell $(CC) -dumpmachine)
ifneq (,$(findstring mingw,$(COMPILER_TARGET)))
	SRC_C += ports/windows/fmode.c
endif

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

include $(TOP)/py/mkrules.mk
