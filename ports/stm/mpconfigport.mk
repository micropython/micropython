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

ifeq ($(MCU_VARIANT),STM32F407xx)
        UF2_FAMILY_ID ?= 0x6d0922fa
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
        CIRCUITPY_RTC ?= 1
        USB_NUM_ENDPOINT_PAIRS = 4
        UF2_FAMILY_ID ?= 0x57755a57
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
        CIRCUITPY_RTC ?= 1

        USB_NUM_ENDPOINT_PAIRS = 9
        UF2_FAMILY_ID ?= 0x6db66082
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
        CIRCUITPY_RTC ?= 1

        USB_NUM_ENDPOINT_PAIRS = 6
        UF2_FAMILY_ID ?= 0x53b80f00
endif

ifeq ($(MCU_SERIES),L4)
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
        CIRCUITPY_RTC ?= 1
        # todo - this varies between devices in the series
        # This slide deck https://www.st.com/content/ccc/resource/training/technical/product_training/98/89/c8/6c/3e/e9/49/79/STM32L4_Peripheral_USB.pdf/files/STM32L4_Peripheral_USB.pdf/jcr:content/translations/en.STM32L4_Peripheral_USB.pdf
        # cites 16 endpoints, 8 endpoint pairs, while section 3.39 of the L4R5 datasheet states 6 endpoint pairs.
        USB_NUM_ENDPOINT_PAIRS = 6
        UF2_FAMILY_ID ?= 0x00ff6919
endif

CIRCUITPY_PARALLELDISPLAY := 0
