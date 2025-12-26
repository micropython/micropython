# Makefile for building dual-mode modules as native modules
# This is a simplified version that doesn't use the auto-generation

MPY_DIR = ../../..
ARCH = x64

# Include paths
CFLAGS = -I. -I$(MPY_DIR)
CFLAGS += -std=c99 -Os -Wall -Werror -DNDEBUG
CFLAGS += -DNO_QSTR -DMICROPY_ENABLE_DYNRUNTIME
CFLAGS += -fpic -fno-common
CFLAGS += -U_FORTIFY_SOURCE

# Simple module
dualmode_simple.o: dualmode_simple.c mp_dualmode.h
	$(CC) $(CFLAGS) -c -o $@ $<

dualmode_simple.mpy: dualmode_simple.o
	python3 $(MPY_DIR)/tools/mpy_ld.py -o $@ -march $(ARCH) $^

# Example module
dualmode_example.o: dualmode_example.c mp_dualmode.h
	$(CC) $(CFLAGS) -c -o $@ $<

dualmode_example.mpy: dualmode_example.o
	python3 $(MPY_DIR)/tools/mpy_ld.py -o $@ -march $(ARCH) $^

# Build all
all: dualmode_simple.mpy dualmode_example.mpy

# Clean
clean:
	rm -f *.o *.mpy

# Test native modules
test: all
	@echo "Testing native modules..."
	cd $(MPY_DIR)/ports/unix && make
	$(MPY_DIR)/ports/unix/build-standard/micropython test_dualmode.py

.PHONY: all clean test