MPY_TOOL_LONGINT_IMPL ?= -mlongint-impl=mpz
LONGINT_IMPL ?= MPZ
INTERNAL_LIBM ?= 1

ifeq ($(MCU_VARIANT),$(filter $(MCU_VARIANT),STM32F405xx STM32F407xx))
        CIRCUITPY_ALARM = 1
        CIRCUITPY_CANIO = 1
        CIRCUITPY_FRAMEBUFFERIO ?= 1
        CIRCUITPY_SDIOIO ?= 1
        # Number of USB endpoint pairs.
        USB_NUM_ENDPOINT_PAIRS = 4
endif

ifeq ($(MCU_SERIES),F4)
        # Audio via PWM
        CIRCUITPY_AUDIOIO = 0
        CIRCUITPY_AUDIOCORE ?= 1
        CIRCUITPY_AUDIOPWMIO ?= 1

        # Not yet implemented common-hal modules:
        CIRCUITPY_AUDIOBUSIO ?= 0
        CIRCUITPY_COUNTIO ?= 0
        CIRCUITPY_FREQUENCYIO ?= 0
        CIRCUITPY_I2CPERIPHERAL ?= 0
        CIRCUITPY_NVM ?= 0
        CIRCUITPY_ROTARYIO ?= 0
        CIRCUITPY_RTC ?= 0
        USB_NUM_ENDPOINT_PAIRS = 4
endif

ifeq ($(MCU_SERIES),H7)
        # Not yet implemented common-hal modules:
        CIRCUITPY_ANALOGIO ?= 0
        CIRCUITPY_AUDIOBUSIO ?= 0
        CIRCUITPY_AUDIOIO ?= 0
        CIRCUITPY_COUNTIO ?= 0
        CIRCUITPY_FREQUENCYIO ?= 0
        CIRCUITPY_I2CPERIPHERAL ?= 0
        CIRCUITPY_NEOPIXEL_WRITE ?= 0
        CIRCUITPY_NVM ?= 0
        CIRCUITPY_PULSEIO ?= 0
        CIRCUITPY_PWMIO ?= 0
        CIRCUITPY_ROTARYIO ?= 0
        CIRCUITPY_RTC ?= 0

        USB_NUM_ENDPOINT_PAIRS = 9
endif

ifeq ($(MCU_SERIES),F7)
        # Not yet implemented common-hal modules:
        CIRCUITPY_ANALOGIO ?= 0
        CIRCUITPY_AUDIOBUSIO ?= 0
        CIRCUITPY_AUDIOIO ?= 0
        CIRCUITPY_COUNTIO ?= 0
        CIRCUITPY_FREQUENCYIO ?= 0
        CIRCUITPY_I2CPERIPHERAL ?= 0
        CIRCUITPY_NEOPIXEL_WRITE ?= 0
        CIRCUITPY_NVM ?= 0
        CIRCUITPY_ROTARYIO ?= 0
        CIRCUITPY_RTC ?= 0

        USB_NUM_ENDPOINT_PAIRS = 6
endif
