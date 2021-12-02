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

SRC_BITMAP := \
	$(patsubst ../../%,%,$(wildcard ../../shared-bindings/gifio/*.c ../../shared-module/gifio/*.c)) \
	shared/runtime/context_manager_helpers.c \
	displayio_min.c \
	shared-bindings/aesio/aes.c \
	shared-bindings/aesio/__init__.c \
	shared-bindings/bitmaptools/__init__.c \
	shared-bindings/displayio/Bitmap.c \
	shared-bindings/rainbowio/__init__.c \
	shared-bindings/traceback/__init__.c \
	shared-bindings/util.c \
	shared-module/aesio/aes.c \
	shared-module/aesio/__init__.c \
	shared-module/bitmaptools/__init__.c \
	shared-module/displayio/area.c \
	shared-module/displayio/Bitmap.c \
	shared-module/displayio/ColorConverter.c \
	shared-module/displayio/ColorConverter.c \
	shared-module/rainbowio/__init__.c \
	shared-module/traceback/__init__.c

$(info $(SRC_BITMAP))
SRC_C += $(SRC_BITMAP)

CFLAGS += \
	-DCIRCUITPY_AESIO=1 \
	-DCIRCUITPY_BITMAPTOOLS=1 \
	-DCIRCUITPY_DISPLAYIO_UNIX=1 \
	-DCIRCUITPY_GIFIO=1 \
	-DCIRCUITPY_RAINBOWIO=1 \
	-DCIRCUITPY_TRACEBACK=1

SRC_C += coverage.c
SRC_CXX += coveragecpp.cpp
