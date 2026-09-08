# prerequisites
#
# WASI_SDK: wasi-sdk 25.0 or later
# WASM_OPT: binaryen wasm-opt version_117 or later

# Note:
# we specify the target "--target=wasm32-wasi" explicitly below for the case
# where $CLANG is built with a configuration different from wasi-sdk.
# ditto for "-B $(WASI_SDK)/bin/".

WASI_SDK = /opt/wasi-sdk-25.0
WASI_SYSROOT = $(WASI_SDK)/share/wasi-sysroot
WASM_OPT = wasm-opt
RESOURCE_DIR = $(shell $(WASI_SDK)/bin/clang --print-resource-dir)
CLANG = $(WASI_SDK)/bin/clang

CC = $(CLANG) --sysroot $(WASI_SYSROOT) -resource-dir $(RESOURCE_DIR)
STRIP = $(WASI_SDK)/bin/strip
SIZE = $(WASI_SDK)/bin/size

CFLAGS += --target=wasm32-wasi -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -mllvm -wasm-enable-sjlj
LDFLAGS += --target=wasm32-wasi -lwasi-emulated-process-clocks -lwasi-emulated-signal -lwasi-emulated-mman -lsetjmp -B $(WASI_SDK)/bin/

# WASI doesn't have FFI
MICROPY_PY_FFI = 0

# When threading is enabled, micropython GC uses signals, which is
# not available on WASI.
MICROPY_PY_THREAD = 0

# Disable for now because network support is limited in WASI.
MICROPY_PY_SOCKET = 0
MICROPY_PY_SSL = 0

# ../../lib/berkeley-db-1.xx/PORT/include/db.h:40:10:
# fatal error: 'sys/cdefs.h' file not found
MICROPY_PY_BTREE = 0

# WASI doesn't have termios
MICROPY_PY_TERMIOS = 0
MICROPY_USE_READLINE = 0

# The following things might just work as they are.
# Disabled for now because I'm not interested in them.
MICROPY_VFS_FAT = 0
MICROPY_VFS_LFS1 = 0
MICROPY_VFS_LFS2 = 0

PROG ?= micropython

# LLVM still uses the older version of EH proposal. ("phase 3")
# Convert to the latest version of EH proposal with exnref.
$(BUILD)/$(PROG).spilled.exnref: $(BUILD)/$(PROG).spilled
	$(WASM_OPT) --translate-to-exnref --enable-exception-handling \
	-o $(BUILD)/$(PROG).spilled.exnref $(BUILD)/$(PROG).spilled

# Unlike emscripten, WASI doesn't provide a way to scan GC roots
# like WASM locals. Hopefully --spill-pointers can workaround it.
$(BUILD)/$(PROG).spilled: $(BUILD)/$(PROG)
	$(WASM_OPT) --spill-pointers -o $(BUILD)/$(PROG).spilled $(BUILD)/$(PROG)

all: $(BUILD)/$(PROG).spilled.exnref
