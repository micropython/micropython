MPY_DIR = ../../../..
MPY_CROSS = $(MPY_DIR)/mpy-cross/build/mpy-cross

SRC = module.py
ARCH = rv32imc
PYTHON := $(command -v python3 2> /dev/null)
ifndef PYTHON
PYTHON = python
endif

.PHONY: all
all:
	@$(MPY_CROSS) -march=$(ARCH) $(SRC) -o - | $(PYTHON) -c 'import sys; print(sys.stdin.buffer.read())'
