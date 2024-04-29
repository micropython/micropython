LONGINT_IMPL = MPZ
INTERNAL_LIBM = 1

# We build .elf files to run in the simulator. Generally, this will be a file
# type supported by the chip family's bootloader.
CIRCUITPY_BUILD_EXTENSIONS = elf

# The port manages flash itself instead of using SPI flash via busio.SPI.
INTERNAL_FLASH_FILESYSTEM = 1

# Usually lots of flash space available
CIRCUITPY_MESSAGE_COMPRESSION_LEVEL = 1

# Disable modules included in full builds
CIRCUITPY_FULL_BUILD = 0

# Most ports will want to enable this early to use the USB workflow in testing
# module implementation. It depends on TinyUSB supporting the USB peripheral.
# Renode uses UART instead of USB.
CIRCUITPY_USB_DEVICE = 0

# Disable modules included in slim builds directly. Enable these as they are
# implemented.
CIRCUITPY_ANALOGBUFIO = 0
CIRCUITPY_ANALOGIO = 0
CIRCUITPY_BUSIO_I2C = 0
CIRCUITPY_BUSIO_SPI = 0
CIRCUITPY_DIGITALIO = 0
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_NVM = 0
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_PWMIO = 0
CIRCUITPY_RANDOM = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_ROTARYIO_SOFTENCODER = 0
CIRCUITPY_RTC = 0
CIRCUITPY_STORAGE = 0
CIRCUITPY_TOUCHIO = 0

CIRCUITPY_BITBANGIO = $(CIRCUITPY_DIGITALIO)
