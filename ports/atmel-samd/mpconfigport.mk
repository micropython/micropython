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
CIRCUITPY_AUDIOMIXER = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_TOUCHIO_USE_NATIVE = 1

# SAMD21 needs separate endpoint pairs for MSC BULK IN and BULK OUT, otherwise it's erratic.
USB_MSC_EP_NUM_OUT = 1
endif

# Put samd51-only choices here.
ifeq ($(CHIP_FAMILY),samd51)
CIRCUITPY_SAMD = 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
endif

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 32
