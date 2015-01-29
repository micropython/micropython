include ../py/mkenv.mk
-include mpconfigport.mk

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include ../py/py.mk

INC =  -I.
INC +=  -I..
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Wpointer-arith -Werror
CFLAGS = $(INC) $(CWARN) -ansi -std=gnu99 -DUNIX $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of micropython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and 
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
CC = clang
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref
endif
LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

ifeq ($(MICROPY_FORCE_32BIT),1)
# Note: you may need to install i386 versions of dependency packages,
# starting with linux-libc-dev:i386
CFLAGS += -m32
LDFLAGS += -m32
ifeq ($(MICROPY_PY_FFI),1)
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
ifeq ($(MICROPY_PY_TIME),1)
CFLAGS_MOD += -DMICROPY_PY_TIME=1
SRC_MOD += modtime.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS_MOD += -DMICROPY_PY_TERMIOS=1
SRC_MOD += modtermios.c
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS_MOD += -DMICROPY_PY_SOCKET=1
SRC_MOD += modsocket.c
endif
ifeq ($(MICROPY_PY_FFI),1)
LIBFFI_LDFLAGS_MOD := $(shell pkg-config --libs libffi)
LIBFFI_CFLAGS_MOD := $(shell pkg-config --cflags libffi)
CFLAGS_MOD += $(LIBFFI_CFLAGS_MOD) -DMICROPY_PY_FFI=1
ifeq ($(UNAME_S),Linux)
LDFLAGS_MOD += -ldl
endif
LDFLAGS_MOD += $(LIBFFI_LDFLAGS_MOD)
SRC_MOD += modffi.c
endif


# source files
SRC_C = \
	main.c \
	gccollect.c \
	input.c \
	file.c \
	modos.c \
	alloc.c \
	$(SRC_MOD)


OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

include ../py/mkrules.mk

.PHONY: test

test: $(PROG) ../tests/run-tests
	$(eval DIRNAME=$(notdir $(CURDIR)))
	cd ../tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(PROG) ./run-tests

# install micropython in /usr/local/bin
TARGET = micropython
PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin
PIPSRC = ../tools/pip-micropython
PIPTARGET = pip-micropython

install: micropython
	install -D $(TARGET) $(BINDIR)/$(TARGET)
	install -D $(PIPSRC) $(BINDIR)/$(PIPTARGET)

# uninstall micropython
uninstall:
	-rm $(BINDIR)/$(TARGET)
	-rm $(BINDIR)/$(PIPTARGET)

# build synthetically fast interpreter for benchmarking
fast:
	@echo Make sure to run make -B
	$(MAKE) COPT="-O2 -DNDEBUG -fno-crossjumping" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_fast.h>"' BUILD=build-fast PROG=micropython_fast

# build a minimal interpreter
minimal:
	@echo Make sure to run make -B
	$(MAKE) COPT="-Os -DNDEBUG" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_minimal.h>"' BUILD=build-minimal PROG=micropython_minimal MICROPY_PY_TIME=0 MICROPY_PY_TERMIOS=0 MICROPY_PY_SOCKET=0 MICROPY_PY_FFI=0

# build an interpreter for coverage testing and do the testing
coverage:
	@echo Make sure to run make -B
	$(MAKE) COPT="-O0" CFLAGS_EXTRA='-fprofile-arcs -ftest-coverage' LDFLAGS_EXTRA='-fprofile-arcs -ftest-coverage' BUILD=build-coverage PROG=micropython_coverage
	$(eval DIRNAME=$(notdir $(CURDIR)))
	cd ../tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests
	gcov -o build-coverage/py ../py/*.c
