MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz
LONGINT_IMPL = MPZ
INTERNAL_LIBM = 1
USB_SERIAL_NUMBER_LENGTH = 24

ifeq ($(MCU_SERIES),F4)
	# Items requiring common-hal implementation:
	CIRCUITPY_AUDIOBUSIO = 0
	CIRCUITPY_AUDIOIO = 0
	CIRCUITPY_ROTARYIO = 0
	CIRCUITPY_RTC = 0
	CIRCUITPY_FREQUENCYIO = 0
	CIRCUITPY_I2CSLAVE = 0
	# Can be overridden on board level
	ifndef CIRCUITPY_NVM
		CIRCUITPY_NVM = 0
	endif
endif

ifeq ($(MCU_SERIES),H7)
	# Items requiring common-hal implementation:
	CIRCUITPY_ANALOGIO = 0
	CIRCUITPY_NEOPIXEL_WRITE = 0
	CIRCUITPY_PULSEIO = 0
	CIRCUITPY_OS = 0
	CIRCUITPY_NVM = 0
	CIRCUITPY_AUDIOBUSIO = 0
	CIRCUITPY_AUDIOIO = 0
	CIRCUITPY_ROTARYIO = 0
	CIRCUITPY_RTC = 0
	CIRCUITPY_FREQUENCYIO = 0
	CIRCUITPY_I2CSLAVE = 0
	# Modules with no common-hal implementation that depend on something else
	CIRCUITPY_DISPLAYIO = 0 # Requires SPI, PulseIO, and common-hal module (stub ok)
	CIRCUITPY_RANDOM = 0	# Requires OS
	CIRCUITPY_STORAGE = 0	# Requires OS, filesystem
endif

ifeq ($(MCU_SERIES),F7)
	# Items requiring common-hal implementation:
	CIRCUITPY_ANALOGIO = 0
	CIRCUITPY_NEOPIXEL_WRITE = 0
	CIRCUITPY_PULSEIO = 0
	CIRCUITPY_OS = 0
	CIRCUITPY_NVM = 0
	CIRCUITPY_AUDIOBUSIO = 0
	CIRCUITPY_AUDIOIO = 0
	CIRCUITPY_ROTARYIO = 0
	CIRCUITPY_RTC = 0
	CIRCUITPY_FREQUENCYIO = 0
	CIRCUITPY_I2CSLAVE = 0
	# Modules with no common-hal implementation that depend on something else
	CIRCUITPY_DISPLAYIO = 0 # Requires SPI, PulseIO, and common-hal module (stub ok)
	CIRCUITPY_RANDOM = 0	# Requires OS
	CIRCUITPY_STORAGE = 0	# Requires OS, filesystem
endif
