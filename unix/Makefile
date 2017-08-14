-include mpconfigport.mk
include ../py/mkenv.mk

# define main target
PROG = micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include ../py/py.mk

INC +=  -I.
INC +=  -I..
INC += -I../lib/timeutils
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -ansi -std=gnu99 -DUNIX $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os -fdata-sections -ffunction-sections #-DNDEBUG
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
# The unix port of micropython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and 
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
CC = clang
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
INC +=  -I../lib/mp-readline
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
LIB_SRC_C_EXTRA += mp-readline/readline.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS_MOD += -DMICROPY_PY_TERMIOS=1
SRC_MOD += modtermios.c
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS_MOD += -DMICROPY_PY_SOCKET=1
SRC_MOD += modsocket.c
endif
ifeq ($(MICROPY_PY_THREAD),1)
CFLAGS_MOD += -DMICROPY_PY_THREAD=1 -DMICROPY_PY_THREAD_GIL=0
LDFLAGS_MOD += -lpthread
endif

ifeq ($(MICROPY_PY_FFI),1)

ifeq ($(MICROPY_STANDALONE),1)
LIBFFI_CFLAGS_MOD := -I$(shell ls -1d ../lib/libffi/build_dir/out/lib/libffi-*/include)
 ifeq ($(MICROPY_FORCE_32BIT),1)
  LIBFFI_LDFLAGS_MOD = ../lib/libffi/build_dir/out/lib32/libffi.a
 else
  LIBFFI_LDFLAGS_MOD = ../lib/libffi/build_dir/out/lib/libffi.a
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
	modtime.c \
	moduselect.c \
	alloc.c \
	coverage.c \
	fatfs_port.c \
	$(SRC_MOD)

# Include builtin package manager in the standard build (and coverage)
ifeq ($(PROG),micropython)
SRC_C += $(BUILD)/_frozen_upip.c
else ifeq ($(PROG),micropython_coverage)
SRC_C += $(BUILD)/_frozen_upip.c
else ifeq ($(PROG), micropython_nanbox)
SRC_C += $(BUILD)/_frozen_upip.c
else ifeq ($(PROG), micropython_freedos)
SRC_C += $(BUILD)/_frozen_upip.c
endif

LIB_SRC_C = $(addprefix lib/,\
	$(LIB_SRC_C_EXTRA) \
	timeutils/timeutils.c \
	)

# FatFS VFS support
LIB_SRC_C += $(addprefix lib/,\
	fatfs/ff.c \
	fatfs/option/ccsbcs.c \
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
MPY_CROSS = ../mpy-cross/mpy-cross
MPY_TOOL = ../tools/mpy-tool.py
FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE=0 # not supported
CFLAGS += -DMPZ_DIG_SIZE=16 # force 16 bits to work on both 32 and 64 bit archs
OBJ += $(BUILD)/$(BUILD)/frozen_mpy.o

# to build .mpy files from .py files
$(BUILD)/$(FROZEN_MPY_DIR)/%.mpy: $(FROZEN_MPY_DIR)/%.py
	@$(ECHO) "MPY $<"
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(MPY_CROSS) -o $@ -s $(^:$(FROZEN_MPY_DIR)/%=%) $^

# to build frozen_mpy.c from all .mpy files
$(BUILD)/frozen_mpy.c: $(FROZEN_MPY_MPY_FILES) $(BUILD)/genhdr/qstrdefs.generated.h
	@$(ECHO) "Creating $@"
	$(Q)$(PYTHON) $(MPY_TOOL) -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h $(FROZEN_MPY_MPY_FILES) > $@
endif


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
	install -d $(BINDIR)
	install $(TARGET) $(BINDIR)/$(TARGET)
	install $(PIPSRC) $(BINDIR)/$(PIPTARGET)

# uninstall micropython
uninstall:
	-rm $(BINDIR)/$(TARGET)
	-rm $(BINDIR)/$(PIPTARGET)

# build synthetically fast interpreter for benchmarking
fast:
	$(MAKE) COPT="-O2 -DNDEBUG -fno-crossjumping" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_fast.h>"' BUILD=build-fast PROG=micropython_fast

# build a minimal interpreter
minimal:
	$(MAKE) COPT="-Os -DNDEBUG" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_minimal.h>"' \
	    BUILD=build-minimal PROG=micropython_minimal \
	    MICROPY_PY_BTREE=0 MICROPY_PY_FFI=0 MICROPY_PY_SOCKET=0 MICROPY_PY_THREAD=0 \
	    MICROPY_PY_TERMIOS=0 MICROPY_PY_USSL=0 \
	    MICROPY_USE_READLINE=0 MICROPY_FATFS=0

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
	CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_freedos.h>" -DMICROPY_NLR_SETJMP -Dtgamma=gamma -DMICROPY_EMIT_X86=0 -DMICROPY_NO_ALLOCA=1 -DMICROPY_PY_USELECT=0' \
	BUILD=build-freedos \
	PROG=micropython_freedos \
	MICROPY_PY_SOCKET=0 \
	MICROPY_PY_FFI=0 \
	MICROPY_PY_JNI=0

# build an interpreter for coverage testing and do the testing
coverage:
	$(MAKE) COPT="-O0" CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_coverage.h>" -fprofile-arcs -ftest-coverage -Wdouble-promotion -Wformat -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition -Wpointer-arith -Wshadow -Wsign-compare -Wuninitialized -Wunused-parameter -DMICROPY_UNIX_COVERAGE' LDFLAGS_EXTRA='-fprofile-arcs -ftest-coverage' BUILD=build-coverage PROG=micropython_coverage

coverage_test: coverage
	$(eval DIRNAME=$(notdir $(CURDIR)))
	cd ../tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests
	cd ../tests && MICROPY_MICROPYTHON=../$(DIRNAME)/micropython_coverage ./run-tests --emit native
	gcov -o build-coverage/py ../py/*.c
	gcov -o build-coverage/extmod ../extmod/*.c

$(BUILD)/_frozen_upip.c: $(BUILD)/frozen_upip/upip.py
	$(MAKE_FROZEN) $(dir $^) > $@

# Select latest upip version available
UPIP_TARBALL := $(shell ls -1 -v ../tools/micropython-upip-*.tar.gz | tail -n1)

$(BUILD)/frozen_upip/upip.py: $(UPIP_TARBALL)
	$(ECHO) "MISC Preparing upip as frozen module"
	$(Q)mkdir -p $(BUILD)
	$(Q)rm -rf $(BUILD)/micropython-upip-*
	$(Q)tar -C $(BUILD) -xz -f $^
	$(Q)rm -rf $(dir $@)
	$(Q)mkdir -p $(dir $@)
	$(Q)cp $(BUILD)/micropython-upip-*/upip*.py $(dir $@)


# Value of configure's --host= option (required for cross-compilation).
# Deduce it from CROSS_COMPILE by default, but can be overriden.
ifneq ($(CROSS_COMPILE),)
CROSS_COMPILE_HOST = --host=$(patsubst %-,%,$(CROSS_COMPILE))
else
CROSS_COMPILE_HOST =
endif

deplibs: libffi axtls

# install-exec-recursive & install-data-am targets are used to avoid building
# docs and depending on makeinfo
libffi:
	cd ../lib/libffi; git clean -d -x -f
	cd ../lib/libffi; ./autogen.sh
	mkdir -p ../lib/libffi/build_dir; cd ../lib/libffi/build_dir; \
	../configure $(CROSS_COMPILE_HOST) --prefix=$$PWD/out --disable-structs CC="$(CC)" CXX="$(CXX)" LD="$(LD)" CFLAGS="-Os -fomit-frame-pointer -fstrict-aliasing -ffast-math -fno-exceptions"; \
	make install-exec-recursive; make -C include install-data-am

axtls: $(BUILD)/libaxtls.a

$(BUILD)/libaxtls.a: ../lib/axtls/README | $(OBJ_DIRS)
	cd ../lib/axtls; cp config/upyconfig config/.config
	cd ../lib/axtls; make oldconfig -B
	cd ../lib/axtls; make clean
	cd ../lib/axtls; make all CC="$(CC)" LD="$(LD)"
	cp ../lib/axtls/_stage/libaxtls.a $@

../lib/axtls/README:
	@echo "You cloned without --recursive, fetching submodules for you."
	(cd ..; git submodule update --init --recursive)
