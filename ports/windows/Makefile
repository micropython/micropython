include ../../py/mkenv.mk
-include mpconfigport.mk

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = ../unix/qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

# compiler settings
CFLAGS = $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Werror -std=gnu99 -DUNIX -D__USE_MINGW_ANSI_STDIO=1 $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)
LDFLAGS = $(LDFLAGS_MOD) -lm $(LDFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
endif

# source files
SRC_C = \
	shared/libc/printf.c \
	shared/runtime/gchelper_generic.c \
	ports/unix/main.c \
	ports/unix/input.c \
	ports/unix/modos.c \
	ports/unix/modmachine.c \
	ports/unix/modtime.c \
	ports/unix/gccollect.c \
	windows_mphal.c \
	realpath.c \
	init.c \
	sleep.c \
	fmode.c \
	$(SRC_MOD)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

ifeq ($(MICROPY_USE_READLINE),1)
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
SRC_C += shared/readline/readline.c
endif

LIB += -lws2_32

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

ifneq ($(FROZEN_MANIFEST),)
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool -DMICROPY_MODULE_FROZEN_MPY=1 -DMPZ_DIG_SIZE=16
endif

include $(TOP)/py/mkrules.mk
