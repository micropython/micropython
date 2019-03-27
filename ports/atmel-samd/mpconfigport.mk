# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.

ifeq ($(LONGINT_IMPL),NONE)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=none
endif

ifeq ($(LONGINT_IMPL),MPZ)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz
endif

ifeq ($(LONGINT_IMPL),LONGLONG)
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=longlong
endif

# Put samd21-only choices here.
ifeq ($(CHIP_FAMILY),samd21)
# frequencyio not yet verified as working on SAMD21.
CIRCUITPY_FRQUENCYIO = 0
endif

# Put samd51-only choices here.
ifeq ($(CHIP_FAMILY),samd51)
CIRCUITPY_SAMD = 1
endif

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 32
