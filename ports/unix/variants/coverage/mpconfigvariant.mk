# Disable optimisations and enable assert() on coverage builds.
DEBUG ?= 1

CFLAGS += \
	-fprofile-arcs -ftest-coverage \
	-Wformat -Wmissing-declarations -Wmissing-prototypes \
	-Wold-style-definition -Wpointer-arith -Wshadow -Wuninitialized -Wunused-parameter \
	-DMICROPY_UNIX_COVERAGE

LDFLAGS += -fprofile-arcs -ftest-coverage

FROZEN_MANIFEST ?= $(VARIANT_DIR)/manifest.py
USER_C_MODULES = $(TOP)/examples/usercmodule

# Optionally format the generated files with uncrustify if installed
ifneq ($(shell which uncrustify-apple-macos-gcc),)
MKAPIFORMAT = --format
endif
include $(TOP)/extmod/multiversal.mk
$(eval $(call multiversal_module,mkapitest,variants/coverage/mkapi_test.yaml))
$(info $(call multiversal_module,mkapitest,variants/coverage/mkapi_test.yaml))

SRC_C += coverage.c
SRC_CXX += coveragecpp.cpp
