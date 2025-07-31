MPY_DIR = ../../..

MOD = test_$(ARCH)
SRC = test.c
ARCH = x64

.PHONY: main
main: all
	$(Q)cat $(MOD).mpy | python -c 'import sys; print(sys.stdin.buffer.read())'

include $(MPY_DIR)/py/dynruntime.mk
