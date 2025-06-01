# Disable optimisations and enable assert() on coverage builds.
DEBUG ?= 1

SANITIZER ?= -fsanitize=undefined

CFLAGS += \
	-fprofile-arcs -ftest-coverage \
	-Wformat -Wmissing-declarations -Wmissing-prototypes \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	-DMICROPY_UNIX_COVERAGE $(SANITIZER)

LDFLAGS += -fprofile-arcs -ftest-coverage $(SANITIZER)

FROZEN_MANIFEST ?= $(VARIANT_DIR)/manifest.py
USER_C_MODULES = $(TOP)/examples/usercmodule

SRC_C += coverage.c
SRC_CXX += coveragecpp.cpp
