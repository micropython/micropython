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

# All raspberrypi ports have longints.
LONGINT_IMPL = MPZ

CIRCUITPY_RP2PIO ?= 1
CIRCUITPY_NEOPIXEL_WRITE ?= $(CIRCUITPY_RP2PIO)
CIRCUITPY_FRAMEBUFFERIO ?= 1
CIRCUITPY_FULL_BUILD ?= 1
CIRCUITPY_AUDIOMP3 ?= 1
CIRCUITPY_BITOPS ?= 1
CIRCUITPY_PWMIO ?= 1
CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_ROTARYIO ?= 1

# Things that need to be implemented.
# Use PWM interally
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_I2CPERIPHERAL = 0
CIRCUITPY_NVM = 1
# Use PIO interally
CIRCUITPY_PULSEIO ?= 1
CIRCUITPY_WATCHDOG ?= 1

# Audio via PWM
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOCORE ?= 1
CIRCUITPY_AUDIOPWMIO ?= 1

# These libraries require Cortex M4+ for fancy math instructions.
CIRCUITPY_AUDIOMIXER ?= 0

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 16

# Number of USB endpoint pairs.
USB_NUM_EP = 8

INTERNAL_FLASH_FILESYSTEM = 1
