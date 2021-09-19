PROG ?= micropython-coverage

# Disable optimisations and enable assert() on coverage builds.
DEBUG ?= 1

CFLAGS += \
	-fprofile-arcs -ftest-coverage \
	-Wformat -Wmissing-declarations -Wmissing-prototypes \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	-DMICROPY_UNIX_COVERAGE \
	-DMODULE_CEXAMPLE_ENABLED=1 -DMODULE_CPPEXAMPLE_ENABLED=1

LDFLAGS += -fprofile-arcs -ftest-coverage

USER_C_MODULES = $(TOP)/examples/usercmodule

MICROPY_VFS_FAT = 1
MICROPY_VFS_LFS1 = 1
MICROPY_VFS_LFS2 = 1

FROZEN_DIR=variants/coverage/frzstr
FROZEN_MPY_DIR=variants/coverage/frzmpy

SRC_QRIO := $(patsubst ../../%,%,$(wildcard ../../shared-bindings/qrio/*.c ../../shared-module/qrio/*.c ../../lib/quirc/lib/*.c))
SRC_C += $(SRC_QRIO)

CFLAGS += -DCIRCUITPY_QRIO=1
$(BUILD)/lib/quirc/lib/%.o: CFLAGS += -Wno-shadow -Wno-sign-compare -include shared-module/qrio/quirc_alloc.h

SRC_C += coverage.c
SRC_CXX += coveragecpp.cpp
