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

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 32

# Number of USB endpoint pairs.
USB_NUM_EP = 8

######################################################################
# Put samd21-only choices here.

ifeq ($(CHIP_FAMILY),samd21)

# The ?='s allow overriding in mpconfigboard.mk.

CIRCUITPY_AUDIOMIXER ?= 0
CIRCUITPY_BINASCII ?= 0
CIRCUITPY_AUDIOMP3 ?= 0
CIRCUITPY_BUILTINS_POW3 ?= 0
CIRCUITPY_FREQUENCYIO ?= 0
CIRCUITPY_JSON ?= 0
CIRCUITPY_TOUCHIO_USE_NATIVE ?= 1

# No room for HCI _bleio on SAMD21.
CIRCUITPY_BLEIO_HCI = 0

CIRCUITPY_SDCARDIO ?= 0

# Not enough RAM for framebuffers
CIRCUITPY_FRAMEBUFFERIO ?= 0

# SAMD21 needs separate endpoint pairs for MSC BULK IN and BULK OUT, otherwise it's erratic.
# Because of that, there aren't enough endpoints for a secondary CDC serial connection.
USB_MSC_EP_NUM_OUT = 1
CIRCUITPY_USB_CDC = 0

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
######################################################################

######################################################################
# Put samd51-only choices here.

ifeq ($(CHIP_FAMILY),samd51)

# No native touchio on SAMD51.
CIRCUITPY_TOUCHIO_USE_NATIVE = 0

# The ?='s allow overriding in mpconfigboard.mk.

CIRCUITPY_NETWORK ?= 0
CIRCUITPY_PS2IO ?= 1
CIRCUITPY_SAMD ?= 1
CIRCUITPY_RGBMATRIX ?= $(CIRCUITPY_FULL_BUILD)
CIRCUITPY_FRAMEBUFFERIO ?= $(CIRCUITPY_FULL_BUILD)

endif # samd51
######################################################################
