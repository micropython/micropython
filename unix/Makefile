include ../py/mkenv.mk
-include mpconfigport.mk

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

INC =  -I.
INC += -I$(PY_SRC)
INC += -I$(BUILD)/includes

# compiler settings
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -DUNIX $(CFLAGS_MOD) $(COPT)

UNAME_S := $(shell uname -s)
 ifeq ($(UNAME_S),Darwin)
    LDFLAGS = $(LDFLAGS_MOD) -lm -Wl,-map,$@.map
else
    LDFLAGS = $(LDFLAGS_MOD) -lm -Wl,-Map=$@.map,--cref
 endif

ifeq ($(MICROPY_FORCE_32BIT),1)
CFLAGS += -m32
LDFLAGS += -m32
ifeq ($(MICROPY_MOD_FFI),1)
ifeq ($(UNAME_S),Linux)
CFLAGS_MOD += -I/usr/include/i686-linux-gnu
endif
endif
endif

ifeq ($(MICROPY_USE_READLINE),1)
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
LDFLAGS_MOD += -lreadline
# the following is needed for BSD
#LDFLAGS_MOD += -ltermcap
endif
ifeq ($(MICROPY_MOD_TIME),1)
CFLAGS_MOD += -DMICROPY_MOD_TIME=1
SRC_MOD += modtime.c
endif
ifeq ($(MICROPY_MOD_FFI),1)
CFLAGS_MOD += `pkg-config --cflags libffi` -DMICROPY_MOD_FFI=1
LDFLAGS_MOD += -ldl -lffi
SRC_MOD += modffi.c
endif


# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
endif

# source files
SRC_C = \
	main.c \
	gccollect.c \
	file.c \
	modsocket.c \
	$(SRC_MOD)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

include ../py/mkrules.mk

.PHONY: test

test: $(PROG) ../tests/run-tests
	$(eval DIRNAME=$(notdir $(CURDIR)))
	cd ../tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(PROG) ./run-tests
