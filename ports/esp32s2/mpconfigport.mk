# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 1

# Chip supplied serial number, in bytes
USB_SERIAL_NUMBER_LENGTH = 12

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

# These modules are implemented in ports/<port>/common-hal:
CIRCUITPY_FULL_BUILD = 1
CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_CANIO = 1
CIRCUITPY_COUNTIO = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_I2CPERIPHERAL = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_NVM = 0
# We don't have enough endpoints to include MIDI.
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_WIFI = 1
CIRCUITPY_ESPIDF = 1

ifndef CIRCUITPY_TOUCHIO_USE_NATIVE
CIRCUITPY_TOUCHIO_USE_NATIVE = 1
endif

CIRCUITPY_MODULE ?= none
