MPY_DIR = ../../..

MOD = test_$(ARCH)
SRC = test.c
ARCH = x64
PYTHON := $(command -v python3 2> /dev/null)
ifndef PYTHON
PYTHON = python
endif

.PHONY: main
main: all
	$(Q)cat $(MOD).mpy | $(PYTHON) -c 'import sys; print(sys.stdin.buffer.read())'

include $(MPY_DIR)/py/dynruntime.mk
