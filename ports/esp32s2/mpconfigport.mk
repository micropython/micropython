# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 1

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

# These modules are implemented in ports/<port>/common-hal:
CIRCUITPY_FULL_BUILD ?= 1
CIRCUITPY_ALARM ?= 1
CIRCUITPY_AUDIOCORE ?= 1
CIRCUITPY_AUDIOMP3 ?= 0
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOBUSIO_PDMIN ?= 0
CIRCUITPY_AUDIOBUSIO_I2SOUT ?= 1
CIRCUITPY_AUDIOIO ?= 0
CIRCUITPY_AUDIOMIXER ?= 1
CIRCUITPY_CANIO ?= 1
CIRCUITPY_COUNTIO ?= 1
CIRCUITPY_DUALBANK ?= 1
CIRCUITPY_FRAMEBUFFERIO ?= 1
CIRCUITPY_FREQUENCYIO ?= 1
CIRCUITPY_IMAGECAPTURE ?= 1
CIRCUITPY_I2CPERIPHERAL ?= 0
CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_ROTARYIO ?= 1
CIRCUITPY_NVM ?= 1
CIRCUITPY_PS2IO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE ?= 1
CIRCUITPY_WIFI ?= 1
CIRCUITPY_WATCHDOG ?= 1

CIRCUITPY_ESPIDF ?= 1

CIRCUITPY_MODULE ?= none

# From the ESP32-S2 datasheet:
#
# Endpoint number 0 always present (bi-directional, consisting of EP0 IN and EP0 OUT)
# Six additional endpoints (endpoint numbers 1 to 6), configurable as IN or OUT
# Maximum of five IN endpoints concurrently active at any time (including EP0 IN)
#
# Due to the limited number of endpoints, some USB devices will be off by default.
# For instance MIDI is available, but the device is turned off. It can be turned on
# only if something else is turned off, such as HID.
USB_NUM_ENDPOINT_PAIRS = 7
USB_NUM_IN_ENDPOINTS = 5
