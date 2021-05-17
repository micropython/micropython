# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 1

# Number of USB endpoint pairs.
USB_NUM_ENDPOINT_PAIRS = 16

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

CIRCUITPY_ANALOGIO = 0
CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_BITBANGIO = 0
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_BOARD = 0
CIRCUITPY_BUSDEVICE = 0
CIRCUITPY_BUSIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_I2CPERIPHERAL = 0
CIRCUITPY_NVM = 0
CIRCUITPY_PWMIO = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_PWMIO = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_RTC = 0
CIRCUITPY_SDCARDIO = 0
# Enable USB support
CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 1
