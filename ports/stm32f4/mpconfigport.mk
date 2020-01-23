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

# The ifndef's allow overriding in mpconfigboard.mk.

ifndef CIRCUITPY_BOARD
CIRCUITPY_BOARD = 1
endif

ifndef CIRCUITPY_DIGITALIO
CIRCUITPY_DIGITALIO = 1
endif

ifndef CIRCUITPY_ANALOGIO
CIRCUITPY_ANALOGIO = 1
endif

ifndef CIRCUITPY_MICROCONTROLLER
CIRCUITPY_MICROCONTROLLER = 1
endif

ifndef CIRCUITPY_BUSIO
CIRCUITPY_BUSIO = 1
endif

ifndef CIRCUITPY_PULSEIO
CIRCUITPY_PULSEIO = 1
endif

ifndef CIRCUITPY_OS
CIRCUITPY_OS = 1
endif

ifndef CIRCUITPY_STORAGE
CIRCUITPY_STORAGE = 1
endif

ifndef CIRCUITPY_RANDOM
CIRCUITPY_RANDOM = 1
endif

ifndef CRICUITPY_USB_HID
CIRCUITPY_USB_HID = 1
endif

ifndef CIRCUITPY_USB_MIDI
CIRCUITPY_USB_MIDI = 1
endif

ifndef CIRCUITPY_NEOPIXEL_WRITE
CIRCUITPY_NEOPIXEL_WRITE = 1
endif

ifndef CIRCUITPY_DISPLAYIO
CIRCUITPY_DISPLAYIO = 1
endif

CFLAGS += -DMICROPY_CPYTHON_COMPAT=1

#ifeq ($(MCU_SUB_VARIANT), stm32f412zx)
#endif
