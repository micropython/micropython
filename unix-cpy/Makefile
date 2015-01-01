include ../py/mkenv.mk

# define main target
PROG = cpy

# include py core make definitions
include ../py/py.mk

INC =  -I.
INC += -I..
INC += -I$(BUILD)

# compiler settings
CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -DUNIX
LDFLAGS = -lm

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -O0 -g
else
CFLAGS += -Os #-DNDEBUG
endif

# source files
SRC_C = \
	main.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
LIB =

include ../py/mkrules.mk

