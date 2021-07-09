# All linking can be done with this common templated linker script, which has
# parameters that vary based on chip and/or board.
LD_TEMPLATE_FILE = boards/common.template.ld

# Define an equivalent for MICROPY_LONGINT_IMPL, to pass to $(MPY-TOOL) in py/mkrules.mk
# $(MPY-TOOL) needs to know what kind of longint to use (if any) to freeze long integers.
# This should correspond to the MICROPY_LONGINT_IMPL definition in mpconfigport.h.
MPY_TOOL_LONGINT_IMPL = -mlongint-impl=mpz

INTERNAL_LIBM = 1

# Number of USB endpoint pairs.
USB_NUM_ENDPOINT_PAIRS = 8

# All nRF ports have longints.
LONGINT_IMPL = MPZ

# The ?='s allow overriding in mpconfigboard.mk.

# Audio via PWM
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOCORE ?= 1
CIRCUITPY_AUDIOMIXER ?= 1
CIRCUITPY_AUDIOPWMIO ?= 1

# Native BLEIO is not compatible with HCI _bleio.
CIRCUITPY_BLEIO_HCI = 0

CIRCUITPY_BLEIO ?= 1

# No I2CPeripheral implementation
CIRCUITPY_I2CPERIPHERAL = 0

CIRCUITPY_RTC ?= 1

# frequencyio not yet implemented
CIRCUITPY_FREQUENCYIO = 0

CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_ROTARYIO_SOFTENCODER = 1
CIRCUITPY_FRAMEBUFFERIO ?= 1

CIRCUITPY_COUNTIO ?= 1
CIRCUITPY_WATCHDOG ?= 1

# Sleep and Wakeup
CIRCUITPY_ALARM ?= 1

# Turn on the BLE file service
CIRCUITPY_BLE_FILE_SERVICE ?= 1

# Turn on the BLE serial service
CIRCUITPY_SERIAL_BLE ?= 1

CIRCUITPY_COMPUTED_GOTO_SAVE_SPACE ?= 1

# nRF52840-specific

ifeq ($(MCU_CHIP),nrf52840)
MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840

# Fits on nrf52840 but space is tight on nrf52833.
CIRCUITPY_AESIO ?= 1

SD ?= s140
SOFTDEV_VERSION ?= 6.1.0

BOOT_SETTING_ADDR = 0xFF000
NRF_DEFINES += -DNRF52840_XXAA -DNRF52840

# CircuitPython doesn't yet support NFC so force the NFC antenna pins to be GPIO.
# See https://github.com/adafruit/circuitpython/issues/1300
# Defined here because system_nrf52840.c doesn't #include any of our own include files.
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS

else
ifeq ($(MCU_CHIP),nrf52833)
MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52833

SD ?= s140
SOFTDEV_VERSION ?= 7.0.1

BOOT_SETTING_ADDR = 0x7F000
NRF_DEFINES += -DNRF52833_XXAA -DNRF52833

endif
endif
