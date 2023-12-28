USB_VID = 0x2886
USB_PID = 0x802F
# VID & PID Provided by Seeed
USB_PRODUCT = "Seeeduino XIAO KB"
USB_MANUFACTURER = "Seeed"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE
CIRCUITPY_FULL_BUILD = 0

# A number of modules are removed
# Many I/O functions are not available or not used in a keyboard
# CIRCUITPY_ANALOGIO = 1 # Needed for potentiometer input (mouse)
CIRCUITPY_AUDIOCORE = 0
# Needed for I2C, SPI and UART - removed that for keyboards...
CIRCUITPY_BUSIO = 0
CIRCUITPY_PULSEIO = 0
# only needed for speaker or LED PWM functions. Takes 2314 bytes.
# CIRCUITPY_PWMIO = 1
CIRCUITPY_RTC = 0
CIRCUITPY_MATH = 0
#CIRCUITPY_RANDOM = 0
CIRCUITPY_ONEWIREIO = 0
# Needed for RGB LEDs
CIRCUITPY_NEOPIXEL_WRITE = 1
# Needed for RGB LEDs
CIRCUITPY_RAINBOWIO = 1
# These are used in a keyboard or computer input device.
CIRCUITPY_ROTARYIO = 1
CIRCUITPY_KEYPAD = 1
CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 1

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
