# All linking can be done with this common templated linker script, which has
# parameters that vary based on chip and/or board.
LD_TEMPLATE_FILE = boards/common.template.ld

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
# frequencyio not yet verified as working on SAMD21, though make it possible to override.
ifndef CIRCUITPY_AUDIOMIXER
CIRCUITPY_AUDIOMIXER = 0
endif

ifndef CIRCUITPY_AUDIOMP3
CIRCUITPY_AUDIOMP3 = 0
endif

ifndef CIRCUITPY_FREQUENCYIO
CIRCUITPY_FREQUENCYIO = 0
endif

ifndef CIRCUITPY_TOUCHIO_USE_NATIVE
CIRCUITPY_TOUCHIO_USE_NATIVE = 1
endif

CIRCUITPY_SDCARDIO ?= 0

# Not enough RAM for framebuffers
CIRCUITPY_FRAMEBUFFERIO ?= 0

# SAMD21 needs separate endpoint pairs for MSC BULK IN and BULK OUT, otherwise it's erratic.
USB_MSC_EP_NUM_OUT = 1

CIRCUITPY_ULAB = 0

ifeq ($(TRANSLATION), ja)
RELEASE_NEEDS_CLEAN_BUILD = 1
CIRCUITPY_TERMINALIO = 0
endif

ifeq ($(TRANSLATION), ko)
RELEASE_NEEDS_CLEAN_BUILD = 1
CIRCUITPY_TERMINALIO = 0
endif

endif # samd21

# Put samd51-only choices here.
ifeq ($(CHIP_FAMILY),samd51)
# No native touchio on SAMD51.
CIRCUITPY_TOUCHIO_USE_NATIVE = 0

# The ifndef's allow overriding in mpconfigboard.mk.

ifndef CIRCUITPY_NETWORK
CIRCUITPY_NETWORK = 0
endif

ifndef CIRCUITPY_PS2IO
CIRCUITPY_PS2IO = 1
endif

ifndef CIRCUITPY_SAMD
CIRCUITPY_SAMD = 1
endif

ifndef CIRCUITPY_RGBMATRIX
CIRCUITPY_RGBMATRIX = $(CIRCUITPY_FULL_BUILD)
endif

ifndef CIRCUITPY_FRAMEBUFFERIO
CIRCUITPY_FRAMEBUFFERIO = $(CIRCUITPY_FULL_BUILD)
endif

endif # samd51

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 32

USB_NUM_EP = 8
