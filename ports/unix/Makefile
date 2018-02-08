-include mpconfigport.mk
include ../../py/mkenv.mk

FROZEN_DIR = scripts
FROZEN_MPY_DIR = modules

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk

INC +=  -I.
INC +=  -I$(TOP)
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -std=gnu99 -DUNIX $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os -fdata-sections -ffunction-sections -DNDEBUG
# _FORTIFY_SOURCE is a feature in gcc/glibc which is intended to provide extra
# security for detecting buffer overflows. Some distros (Ubuntu at the very least)
# have it enabled by default.
#
# gcc already optimizes some printf calls to call puts and/or putchar. When
# _FORTIFY_SOURCE is enabled and compiling with -O1 or greater, then some
# printf calls will also be optimized to call __printf_chk (in glibc). Any
# printfs which get redirected to __printf_chk are then no longer synchronized
# with printfs that go through mp_printf.
#
# In MicroPython, we don't want to use the runtime library's printf but rather
# go through mp_printf, so that stdout is properly tied into streams, etc.
# This means that we either need to turn off _FORTIFY_SOURCE or provide our
# own implementation of __printf_chk. We've chosen to turn off _FORTIFY_SOURCE.
# It should also be noted that the use of printf in MicroPython is typically
# quite limited anyways (primarily for debug and some error reporting, etc
# in the unix version).
#
# Information about _FORTIFY_SOURCE seems to be rather scarce. The best I could
# find was this: https://securityblog.redhat.com/2014/03/26/fortify-and-you/
# Original patchset was introduced by
# https://gcc.gnu.org/ml/gcc-patches/2004-09/msg02055.html .
#
# Turning off _FORTIFY_SOURCE is only required when compiling with -O1 or greater
CFLAGS += -U _FORTIFY_SOURCE
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of MicroPython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
ifeq ($(MICROPY_FORCE_32BIT),1)
CC = clang -m32
else
CC = clang
endif
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map -Wl,-dead_strip
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif
LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

ifeq ($(MICROPY_FORCE_32BIT),1)
# Note: you may need to install i386 versions of dependency packages,
# starting with linux-libc-dev:i386
ifeq ($(MICROPY_PY_FFI),1)
ifeq ($(UNAME_S),Linux)
CFLAGS_MOD += -I/usr/include/i686-linux-gnu
endif
endif
endif

ifeq ($(MICROPY_USE_READLINE),1)
INC +=  -I$(TOP)/lib/mp-readline
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
LIB_SRC_C_EXTRA += mp-readline/readline.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS_MOD += -DMICROPY_PY_TERMIOS=1
SRC_MOD += modtermios.c
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS_MOD += -DMICROPY_PY_SOCKET=1
SRC_MOD += modusocket.c
endif
ifeq ($(MICROPY_PY_THREAD),1)
CFLAGS_MOD += -DMICROPY_PY_THREAD=1 -DMICROPY_PY_THREAD_GIL=0
LDFLAGS_MOD += -lpthread
endif

ifeq ($(MICROPY_PY_FFI),1)

ifeq ($(MICROPY_STANDALONE),1)
LIBFFI_CFLAGS_MOD := -I$(shell ls -1d $(TOP)/lib/libffi/build_dir/out/lib/libffi-*/include)
 ifeq ($(MICROPY_FORCE_32BIT),1)
  LIBFFI_LDFLAGS_MOD = $(TOP)/lib/libffi/build_dir/out/lib32/libffi.a
 else
  LIBFFI_LDFLAGS_MOD = $(TOP)/lib/libffi/build_dir/out/lib/libffi.a
 endif
else
LIBFFI_CFLAGS_MOD := $(shell pkg-config --cflags libffi)
LIBFFI_LDFLAGS_MOD := $(shell pkg-config --libs libffi)
endif

ifeq ($(UNAME_S),Linux)
LIBFFI_LDFLAGS_MOD += -ldl
endif

CFLAGS_MOD += $(LIBFFI_CFLAGS_MOD) -DMICROPY_PY_FFI=1
LDFLAGS_MOD += $(LIBFFI_LDFLAGS_MOD)
SRC_MOD += modffi.c
endif

ifeq ($(MICROPY_PY_JNI),1)
# Path for 64-bit OpenJDK, should be adjusted for other JDKs
CFLAGS_MOD += -I/usr/lib/jvm/java-7-openjdk-amd64/include -DMICROPY_PY_JNI=1
SRC_MOD += modjni.c
endif

# source files
SRC_C = \
	main.c \
	gccollect.c \
	unix_mphal.c \
	mpthreadport.c \
	input.c \
	file.c \
	modmachine.c \
	modos.c \
	moduos_vfs.c \
	modtime.c \
	moduselect.c \
	alloc.c \
	coverage.c \
	fatfs_port.c \
	$(SRC_MOD)

LIB_SRC_C = $(addprefix lib/,\
	$(LIB_SRC_C_EXTRA) \
	timeutils/timeutils.c \
	)

# FatFS VFS support
LIB_SRC_C += $(addprefix lib/,\
	oofatfs/ff.c \
	oofatfs/option/unicode.c \
	)

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(STMHAL_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(LIB_SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

ifneq ($(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMPZ_DIG_SIZE=16 # force 16 bits to work on both 32 and 64 bit archs
MPY_CROSS_FLAGS += -mcache-lookup-bc
endif


include $(TOP)/py/mkrules.mk

.PHONY: test

test: $(PROG) $(TOP)/tests/run-tests
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(PROG) ./run-tests

# install micropython in /usr/local/bin
TARGET = micropython
PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

install: micropython
	install -d $(BINDIR)
	install $(TARGET) $(BINDIR)/$(TARGET)

# uninstall micropython
uninstall:
	-rm $(BINDIR)/$(TARGET)

# build synthetically fast interpreter for benchmarking
fast:
	$(MAKE) COPT="-O2 -DNDEBUG -fno-crossjumping" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_fast.h>"' BUILD=build-fast PROG=micropython_fast

# build a minimal interpreter
minimal:
	$(MAKE) COPT="-Os -DNDEBUG" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_minimal.h>"' \
	    BUILD=build-minimal PROG=micropython_minimal FROZEN_DIR= FROZEN_MPY_DIR= \
	    MICROPY_PY_BTREE=0 MICROPY_PY_FFI=0 MICROPY_PY_SOCKET=0 MICROPY_PY_THREAD=0 \
	    MICROPY_PY_TERMIOS=0 MICROPY_PY_USSL=0 \
	    MICROPY_USE_READLINE=0

# build interpreter with nan-boxing as object model
nanbox:
	$(MAKE) \
	CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_nanbox.h>"' \
	BUILD=build-nanbox \
	PROG=micropython_nanbox \
	MICROPY_FORCE_32BIT=1 \
	MICROPY_PY_USSL=0

freedos:
	$(MAKE) \
	CC=i586-pc-msdosdjgpp-gcc \
	STRIP=i586-pc-msdosdjgpp-strip \
	SIZE=i586-pc-msdosdjgpp-size \
	CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_freedos.h>" -DMICROPY_NLR_SETJMP -Dtgamma=gamma -DMICROPY_EMIT_X86=0 -DMICROPY_NO_ALLOCA=1 -DMICROPY_PY_USELECT_POSIX=0' \
	BUILD=build-freedos \
	PROG=micropython_freedos \
	MICROPY_PY_SOCKET=0 \
	MICROPY_PY_FFI=0 \
	MICROPY_PY_JNI=0 \
	MICROPY_PY_BTREE=0 \
	MICROPY_PY_THREAD=0 \
	MICROPY_PY_USSL=0

# build an interpreter for coverage testing and do the testing
coverage:
	$(MAKE) \
	    COPT="-O0" CFLAGS_EXTRA='$(CFLAGS_EXTRA) -DMP_CONFIGFILE="<mpconfigport_coverage.h>" \
	    -fprofile-arcs -ftest-coverage \
	    -Wdouble-promotion -Wformat -Wmissing-declarations -Wmissing-prototypes -Wsign-compare \
	    -Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	    -DMICROPY_UNIX_COVERAGE' \
	    LDFLAGS_EXTRA='-fprofile-arcs -ftest-coverage' \
	    FROZEN_DIR=coverage-frzstr FROZEN_MPY_DIR=coverage-frzmpy \
	    BUILD=build-coverage PROG=micropython_coverage

coverage_test: coverage
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests -d thread
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests --emit native
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests --via-mpy -d basics float
	cat $(TOP)/tests/basics/0prelim.py | ./micropython_coverage | grep -q 'abc'
	gcov -o build-coverage/py $(TOP)/py/*.c
	gcov -o build-coverage/extmod $(TOP)/extmod/*.c

# Value of configure's --host= option (required for cross-compilation).
# Deduce it from CROSS_COMPILE by default, but can be overridden.
ifneq ($(CROSS_COMPILE),)
CROSS_COMPILE_HOST = --host=$(patsubst %-,%,$(CROSS_COMPILE))
else
CROSS_COMPILE_HOST =
endif

deplibs: libffi axtls

# install-exec-recursive & install-data-am targets are used to avoid building
# docs and depending on makeinfo
libffi:
	cd $(TOP)/lib/libffi; git clean -d -x -f
	cd $(TOP)/lib/libffi; ./autogen.sh
	mkdir -p $(TOP)/lib/libffi/build_dir; cd $(TOP)/lib/libffi/build_dir; \
	../configure $(CROSS_COMPILE_HOST) --prefix=$$PWD/out --disable-structs CC="$(CC)" CXX="$(CXX)" LD="$(LD)" CFLAGS="-Os -fomit-frame-pointer -fstrict-aliasing -ffast-math -fno-exceptions"; \
	$(MAKE) install-exec-recursive; $(MAKE) -C include install-data-am

axtls: $(BUILD)/libaxtls.a

$(BUILD)/libaxtls.a: $(TOP)/lib/axtls/README | $(OBJ_DIRS)
	cd $(TOP)/lib/axtls; cp config/upyconfig config/.config
	cd $(TOP)/lib/axtls; $(MAKE) oldconfig -B
	cd $(TOP)/lib/axtls; $(MAKE) clean
	cd $(TOP)/lib/axtls; $(MAKE) all CC="$(CC)" LD="$(LD)"
	cp $(TOP)/lib/axtls/_stage/libaxtls.a $@

$(TOP)/lib/axtls/README:
	@echo "You cloned without --recursive, fetching submodules for you."
	(cd $(TOP); git submodule update --init --recursive)
