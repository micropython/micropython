include ../py/mkenv.mk

# define main target
PROG = cpy

# include py core make definitions
include ../py/py.mk

# compiler settings
CFLAGS = -I. -I$(PY_SRC) -Wall -Werror -ansi -std=gnu99 -DUNIX
LDFLAGS = -lm

#Debugging/Optimization
ifdef DEBUG
CFLAGS += -Og -ggdb
else
CFLAGS += -Os #-DNDEBUG
endif

# source files
SRC_C = \
	main.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
LIB =

include ../py/mkrules.mk

