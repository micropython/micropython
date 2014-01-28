include ../py/mkenv.mk

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

# compiler settings
CFLAGS = -I. -I$(PY_SRC) -Wall -Werror -ansi -std=gnu99 -DUNIX
LDFLAGS = -lm -ldl -lffi

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
	ffi.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
LIB = -lreadline
# the following is needed for BSD
#LIB += -ltermcap

include ../py/mkrules.mk

