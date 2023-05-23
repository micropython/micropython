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

FROZEN_MANIFEST ?= $(VARIANT_DIR)/manifest.py
USER_C_MODULES = $(TOP)/examples/usercmodule

MICROPY_VFS_FAT = 1
MICROPY_VFS_LFS1 = 1
MICROPY_VFS_LFS2 = 1

SRC_QRIO := $(patsubst ../../%,%,$(wildcard ../../shared-bindings/qrio/*.c ../../shared-module/qrio/*.c ../../lib/quirc/lib/*.c))
SRC_C += $(SRC_QRIO)

CFLAGS += -DCIRCUITPY_QRIO=1
$(BUILD)/lib/quirc/lib/%.o: CFLAGS += -Wno-shadow -Wno-sign-compare -include shared-module/qrio/quirc_alloc.h

SRC_BITMAP := \
	shared/runtime/context_manager_helpers.c \
	displayio_min.c \
	shared-bindings/aesio/aes.c \
	shared-bindings/aesio/__init__.c \
	shared-bindings/audiocore/__init__.c \
	shared-bindings/audiocore/RawSample.c \
	shared-bindings/audiocore/WaveFile.c \
	shared-bindings/audiomixer/__init__.c \
	shared-bindings/audiomixer/Mixer.c \
	shared-bindings/audiomixer/MixerVoice.c \
	shared-bindings/bitmaptools/__init__.c \
	shared-bindings/displayio/Bitmap.c \
	shared-bindings/rainbowio/__init__.c \
	shared-bindings/struct/__init__.c \
	shared-bindings/synthio/__init__.c \
	shared-bindings/synthio/Math.c \
	shared-bindings/synthio/MidiTrack.c \
	shared-bindings/synthio/LFO.c \
	shared-bindings/synthio/Note.c \
	shared-bindings/synthio/Synthesizer.c \
	shared-bindings/traceback/__init__.c \
	shared-bindings/util.c \
	shared-bindings/zlib/__init__.c \
	shared-module/aesio/aes.c \
	shared-module/aesio/__init__.c \
	shared-module/audiocore/__init__.c \
	shared-module/audiocore/RawSample.c \
	shared-module/audiocore/WaveFile.c \
	shared-module/audiomixer/__init__.c \
	shared-module/audiomixer/Mixer.c \
	shared-module/audiomixer/MixerVoice.c \
	shared-module/bitmaptools/__init__.c \
	shared-module/displayio/area.c \
	shared-module/displayio/Bitmap.c \
	shared-module/displayio/ColorConverter.c \
	shared-module/displayio/ColorConverter.c \
	shared-module/os/getenv.c \
	shared-module/rainbowio/__init__.c \
	shared-module/struct/__init__.c \
	shared-module/synthio/__init__.c \
	shared-module/synthio/Math.c \
	shared-module/synthio/MidiTrack.c \
	shared-module/synthio/LFO.c \
	shared-module/synthio/Note.c \
	shared-module/synthio/Synthesizer.c \
	shared-module/traceback/__init__.c \
	shared-module/zlib/__init__.c \

SRC_C += $(SRC_BITMAP)

CFLAGS += \
	-DCIRCUITPY_AESIO=1 \
	-DCIRCUITPY_AUDIOCORE=1 \
	-DCIRCUITPY_AUDIOMIXER=1 \
	-DCIRCUITPY_AUDIOCORE_DEBUG=1 \
	-DCIRCUITPY_BITMAPTOOLS=1 \
	-DCIRCUITPY_DISPLAYIO_UNIX=1 \
	-DCIRCUITPY_GIFIO=1 \
	-DCIRCUITPY_OS_GETENV=1 \
	-DCIRCUITPY_RAINBOWIO=1 \
	-DCIRCUITPY_STRUCT=1 \
	-DCIRCUITPY_SYNTHIO=1 \
	-DCIRCUITPY_SYNTHIO_MAX_CHANNELS=14 \
	-DCIRCUITPY_TRACEBACK=1 \
	-DCIRCUITPY_ZLIB=1

SRC_C += coverage.c
SRC_CXX += coveragecpp.cpp
