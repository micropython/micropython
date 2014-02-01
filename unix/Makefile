include ../py/mkenv.mk
-include mpconfigport.mk

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

# compiler settings
CFLAGS = -I. -I$(PY_SRC) -Wall -Werror -ansi -std=gnu99 -DUNIX $(CFLAGS_MOD)
LDFLAGS = $(LDFLAGS_MOD) -lm

ifeq ($(MICROPY_MOD_FFI),1)
CFLAGS_MOD += `pkg-config --cflags libffi` -DMICROPY_MOD_FFI=1
LDFLAGS_MOD += -ldl -lffi
SRC_MOD += ffi.c
endif


# Debugging/Optimization
ifdef DEBUG
CFLAGS += -O0 -g
else
CFLAGS += -Os #-DNDEBUG
endif

# source files
SRC_C = \
	main.c \
	file.c \
	socket.c \
	$(SRC_MOD)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
LIB = -lreadline
# the following is needed for BSD
#LIB += -ltermcap

include ../py/mkrules.mk

