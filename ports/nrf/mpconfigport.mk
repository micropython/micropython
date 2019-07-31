# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 16

# All nRF ports have longints.
LONGINT_IMPL = MPZ

# No DAC, so no regular audio.
CIRCUITPY_AUDIOIO = 0

# No I2S yet.
CIRCUITPY_AUDIOBUSIO = 0

# No I2CSlave implementation
CIRCUITPY_I2CSLAVE = 0

# enable NVM
CIRCUITPY_NVM = 1

# enable RTC
CIRCUITPY_RTC = 1

# frequencyio not yet implemented
CIRCUITPY_FREQUENCYIO = 0

# CircuitPython doesn't yet support NFC so force the NFC antenna pins to be GPIO.
# See https://github.com/adafruit/circuitpython/issues/1300
# Defined here because system_nrf52840.c doesn't #include any of our own include files.
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
