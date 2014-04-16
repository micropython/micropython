include ../py/mkenv.mk

# define main target
PROG = micropython.exe

# qstr definitions (must come before including py.mk)
QSTR_DEFS = ../unix/qstrdefsport.h

# include py core make definitions
include ../py/py.mk

INC =  -I.
INC += -I$(PY_SRC)
INC += -I$(BUILD)/includes

# compiler settings
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -DUNIX
LDFLAGS = -lm

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -O0 -g
else
CFLAGS += -Os #-DNDEBUG
endif

# source files
SRC_C = \
	unix/main.c \
	unix/file.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
LIB = -lreadline
LIB += -lws2_32
LIB += -lmman
# the following is needed for BSD
#LIB += -ltermcap

include ../py/mkrules.mk

