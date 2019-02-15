# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

INTERNAL_LIBM = 1

USB_SERIAL_NUMBER_LENGTH = 16

# No DAC, so no regular audio.
CIRCUITPY_AUDIOIO = 0

# No I2S yet.
CIRCUITPY_AUDIOBUSIO = 0

# No I2CSlave implementation
CIRCUITPY_I2CSLAVE = 0

# nvm not yet implemented
CIRCUITPY_NVM = 0

# rtc not yet implemented
CIRCUITPY_RTC = 0
