include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

override CSUPEROPT = -O0

# include py core make definitions
include ../py/py.mk

# emscripten doesn't know how to deal with .S files. So we remove all of the
# nlr files and add back nlrsetjmp.o

PY_O_BASENAME := $(filter-out nlr%, $(PY_O_BASENAME)) nlrsetjmp.o


CC = emcc
CPP = gcc -E

INC += -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I../stmhal
INC += -I$(BUILD)

CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -O0 -DNDEBUG
endif

LD = emcc
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref

LIBS =

SRC_C = \
	main.c \
	uart_core.c \
	uart_extra.c \
	stmhal/printf.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

all: $(BUILD)/micropython.html

$(BUILD)/micropython.bc: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD)/micropython.html: $(BUILD)/micropython.bc
	emcc $^ -o $@

run: $(BUILD)/micropython.html
	emrun $^

test: $(BUILD)/micropython.bc
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include ../py/mkrules.mk


# make CC=emcc CPP='gcc -E' CFLAGS_EXTRA=-DMICROPY_NLR_SETJMP MICROPY_PY_FFI=0 STRIP=echo SIZE=echo V=1
#
#	mv micropython micropython.bc
#	emcc micropython.bc -o micropython.html
#	emrun micropython.html
