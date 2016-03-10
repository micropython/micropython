include ../py/mkenv.mk
-include mpconfigport.mk

# define main target
PROG = micropython.exe

# qstr definitions (must come before including py.mk)
QSTR_DEFS = ../unix/qstrdefsport.h

# include py core make definitions
include ../py/py.mk

INC += -I.
INC += -I..
INC += -I$(BUILD)

# compiler settings
CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -DUNIX -D__USE_MINGW_ANSI_STDIO=1 $(CFLAGS_MOD) $(COPT)
LDFLAGS = $(LDFLAGS_MOD) -lm

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
endif

# source files
SRC_C = \
	unix/main.c \
	unix/file.c \
	unix/input.c \
	unix/modos.c \
	unix/modmachine.c \
	unix/modtime.c \
	unix/gccollect.c \
	windows_mphal.c \
	realpath.c \
	init.c \
	sleep.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

ifeq ($(MICROPY_USE_READLINE),1)
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
SRC_C += lib/mp-readline/readline.c
else ifeq ($(MICROPY_USE_READLINE),2)
CFLAGS_MOD += -DMICROPY_USE_READLINE=2
LDFLAGS_MOD += -lreadline
endif

ifeq ($(CROSS_COMPILE),x86_64-w64-mingw32-)
CFLAGS_MOD += -DMICROPY_NLR_SETJMP=1
endif

LIB += -lws2_32

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

include ../py/mkrules.mk
