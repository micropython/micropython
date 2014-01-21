# define main target
PROG = micropython
all: $(PROG)

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

# program for deletion
RM = /bin/rm
ECHO = @echo

# compiler settings
CC = gcc
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
	file.c \
	socket.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
LIB = -lreadline
# the following is needed for BSD
#LIB += -ltermcap

$(PROG): $(BUILD) $(OBJ)
	$(ECHO) "LINK $<"
	$(Q)$(CC) -o $@ $(OBJ) $(LIB) $(LDFLAGS)
ifndef DEBUG
	$(Q)strip $(PROG)
endif
	$(Q)size $(PROG)

$(BUILD)/%.o: %.c $(QSTR_DEFS)
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/main.o: mpconfigport.h

clean:
	$(RM) -f $(PROG)
	$(RM) -rf $(BUILD)

.PHONY: all clean
