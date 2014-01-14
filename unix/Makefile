# define main target
PROG = py
all: $(PROG)

# include py core make definitions
include ../py/py.mk

# program for deletion
RM = /bin/rm
ECHO = @echo

# compiler settings
CC = gcc
CFLAGS = -I. -I$(PY_SRC) -Wall -Werror -ansi -std=gnu99
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

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(PY_O)
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

$(BUILD)/%.o: %.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/main.o: mpconfigport.h

clean:
	$(RM) -f $(PROG)
	$(RM) -rf $(BUILD)

.PHONY: all clean
