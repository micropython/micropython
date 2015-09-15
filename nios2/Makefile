include ../py/mkenv.mk

CROSS = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE = nios2-elf-
endif

INC =  -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I../stmhal
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
CFLAGS = $(INC) -Wall -ansi -std=gnu99 -fsingle-precision-constant $(COPT)
else
CFLAGS = $(INC) -Wall -ansi -std=gnu99 $(COPT)
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

ifeq ($(CROSS), 1)
LD = nios2-elf-gcc
LDFLAGS = -Wl,-Map=$@.map,--cref
else
LD = gcc
LDFLAGS = -Wl,-Map=$@.map,--cref
endif
LIBS = 

SRC_C = \
	nios2_mphal.c \
	stmhal/printf.c \
	stmhal/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	micropythonlib.c \
	
SRC_S = \
#	\
	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

ifeq ($(MAKELIB), 0)
all: $(BUILD)/firmware.elf
$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@
else ifeq ($(MAKELIB), 1)
all: 
	nios2-elf-ar rcs $(BUILD)/libmicropython.a $(OBJ)
endif

# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include ../py/mkrules.mk
