# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
ifeq ($(CHIP_FAMILY), samd21)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=none
endif

ifeq ($(CHIP_FAMILY), samd51)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz
endif

INTERNAL_LIBM = 1
