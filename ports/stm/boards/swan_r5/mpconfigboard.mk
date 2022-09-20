USB_VID = 0x30A4
USB_PID = 0x02
USB_PRODUCT = "Swan R5"
USB_MANUFACTURER = "Blues Inc."

INTERNAL_FLASH_FILESYSTEM = 1

MCU_SERIES = L4
MCU_VARIANT = STM32L4R5xx
MCU_PACKAGE = WLCSP144

LD_COMMON = boards/common_default.ld
LD_DEFAULT = boards/STM32L4R5_default.ld
# UF2 boot option
LD_BOOT = boards/STM32L4R5_boot.ld
UF2_OFFSET = 0x8010000
UF2_BOOTLOADER ?= 1

# Turn all of the below off while trying to get the thing to run
# These modules are implemented in ports/<port>/common-hal:

# Typically the first module to create
CIRCUITPY_MICROCONTROLLER = 1
CIRCUITPY_ALARM = 1

# Typically the second module to create
CIRCUITPY_DIGITALIO = 1
# Other modules:

CIRCUITPY_OS = 1
CIRCUITPY_STORAGE = 1
CIRCUITPY_USB_MSC = 1
CIRCUITPY_UDB_CDC = 1
CIRCUITPY_USB_VENDOR = 1
CIRCUITPY_NVM = 0

CIRCUITPY_ANALOGIO = 1
CIRCUITPY_BUSIO = 1
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_PULSEIO = 1
CIRCUITPY_PWMIO = 1
CIRCUITPY_AUDIOPWMIO = 1
CIRCUITPY_CANIO = 0
CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_I2CTARGET = 0
# Requires SPI, PulseIO (stub ok):
CIRCUITPY_DISPLAYIO = 0

# These modules are implemented in shared-module/ - they can be included in
# any port once their prerequisites in common-hal are complete.
# Requires DigitalIO:
CIRCUITPY_BITBANGIO = 1
# Requires neopixel_write or SPI (dotstar)
CIRCUITPY_PIXELBUF = 0
# Requires OS
CIRCUITPY_RANDOM = 1
# Requires Microcontroller
CIRCUITPY_TOUCHIO = 1
# Requires USB
CIRCUITPY_USB_HID = 0
CIRCUITPY_USB_MIDI = 0
# Does nothing without I2C
CIRCUITPY_REQUIRE_I2C_PULLUPS = 0
# No requirements, but takes extra flash
CIRCUITPY_ULAB = 1
# requires SPI
CIRCUITPY_SDCARDIO = 0
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_BLEIO = 0
CIRCUITPY_BUSDEVICE = 0
CIRCUITPY_KEYPAD = 1
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_RTC = 1

CIRCUITPY_BUILD_EXTENSIONS = bin,uf2
