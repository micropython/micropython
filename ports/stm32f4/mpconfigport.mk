# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 1

# Chip supplied serial number, in bytes
USB_SERIAL_NUMBER_LENGTH = 24

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

#Reduced feature set for early port
CIRCUITPY_MINIMAL_BUILD = 1

CIRCUITPY_BOARD = 1
CIRCUITPY_DIGITALIO = 1
CIRCUITPY_ANALOGIO = 1
CIRCUITPY_MICROCONTROLLER = 1
CIRCUITPY_BUSIO = 1
CIRCUITPY_OS = 1
CIRCUITPY_STORAGE = 1
CIRCUITPY_RANDOM = 1
CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 1

#ifeq ($(MCU_SUB_VARIANT), stm32f412zx)
#endif
