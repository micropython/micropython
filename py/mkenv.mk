ifneq ($(lastword a b),b)
$(error These Makefiles require make 3.81 or newer)
endif

# Set TOP to be the path to get from the current directory (where make was
# invoked) to the top of the tree. $(lastword $(MAKEFILE_LIST)) returns
# the name of this makefile relative to where make was invoked.
#
# We assume that this file is in the py directory so we use $(dir ) twice
# to get to the top of the tree.

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
TOP := $(patsubst %/py/mkenv.mk,%,$(THIS_MAKEFILE))

include $(TOP)/py/verbose.mk

# default settings; can be overridden in main Makefile

PY_SRC ?= $(TOP)/py
BUILD ?= build

RM = rm
ECHO = @echo
CP = cp
MKDIR = mkdir
SED = sed
CAT = cat
TOUCH = touch
PYTHON = python3
ZIP = zip

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
CXX = $(CROSS_COMPILE)g++
GDB = $(CROSS_COMPILE)gdb
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
AR = $(CROSS_COMPILE)ar
WINDRES = $(CROSS_COMPILE)windres

MAKE_MANIFEST = $(PYTHON) $(TOP)/tools/makemanifest.py
MAKE_FROZEN = $(PYTHON) $(TOP)/tools/make-frozen.py
MPY_TOOL = $(PYTHON) $(TOP)/tools/mpy-tool.py

MPY_LIB_SUBMODULE_DIR = $(TOP)/lib/micropython-lib
MPY_LIB_DIR = $(MPY_LIB_SUBMODULE_DIR)

ifeq ($(MICROPY_MPYCROSS),)
MICROPY_MPYCROSS = $(TOP)/mpy-cross/build/mpy-cross
MICROPY_MPYCROSS_DEPENDENCY = $(MICROPY_MPYCROSS)
endif

all:
.PHONY: all

.DELETE_ON_ERROR:

MKENV_INCLUDED = 1
