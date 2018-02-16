#
# set up some of mkenv.py's settings
all:

.PHONY: all

BUILD ?= $(PKGDIRBASE)/micropython
CROSS_COMPILE = $(PREFIX)

# re-use RIOT's variant of verbose build
ifeq ($(QUIET),1)
BUILD_VERBOSE = 0
else
BUILD_VERBOSE = 1
endif

include $(BUILD)/py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

INC =  -I$(TOP)/ports/riot
INC += -I$(TOP)/lib/mp-readline
#INC += -I../stmhal
INC += -I$(BUILD)
INC += $(INCLUDES)

Wno = -Wno-vla -Wno-unused-parameter -Wno-pedantic

CFLAGS += -std=gnu99 -DNDEBUG $(INC) $(Wno)

FROZEN_DIR = scripts

SRC_C = $(wildcard *.c) \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	ports/unix/gccollect.c \
	$(BUILD)/frozen.c \

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BINDIR)/micropython.a

$(BINDIR)/micropython.a: $(OBJ)
	$(AD)$(AR) $(ARFLAGS) $@ $?

include $(TOP)/py/mkrules.mk
