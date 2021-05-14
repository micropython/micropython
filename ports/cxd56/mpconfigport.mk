USB_HIGHSPEED = 1

# Number of USB endpoint pairs.
USB_NUM_ENDPOINT_PAIRS = 6

# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_CAMERA = 1
CIRCUITPY_COUNTIO = 0
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_GAMEPAD = 0
CIRCUITPY_GNSS = 1
CIRCUITPY_I2CPERIPHERAL = 0
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_NVM = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_SDIOIO = 1
CIRCUITPY_TOUCHIO = 0
CIRCUITPY_USB_HID = 0
CIRCUITPY_USB_MIDI = 0
INTERNAL_LIBM = 1
