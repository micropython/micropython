# Use internal flash for CIRCUITPY drive
INTERNAL_FLASH_FILESYSTEM = 1

# Internal math library is substantially smaller than toolchain one
INTERNAL_LIBM = 1

# Longints can be implemented as mpz, as longlong, or not
LONGINT_IMPL = MPZ

# Enable more features
CIRCUITPY_FULL_BUILD ?= 1

# These modules are implemented in ports/<port>/common-hal:
CIRCUITPY_ALARM ?= 1
CIRCUITPY_ANALOGBUFIO ?= 1
CIRCUITPY_AUDIOBUSIO ?= 1
CIRCUITPY_AUDIOBUSIO_I2SOUT ?= 1
CIRCUITPY_AUDIOBUSIO_PDMIN ?= 0
CIRCUITPY_AUDIOCORE ?= 1
CIRCUITPY_AUDIOIO ?= 0
CIRCUITPY_AUDIOMIXER ?= 1
CIRCUITPY_AUDIOMP3 ?= 0
CIRCUITPY_BLEIO ?= 1
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_CANIO ?= 1
CIRCUITPY_COUNTIO ?= 1
CIRCUITPY_DUALBANK ?= 1
CIRCUITPY_ESPCAMERA ?= 1
CIRCUITPY_ESPIDF ?= 1
CIRCUITPY_ESPULP ?= 1
CIRCUITPY_FRAMEBUFFERIO ?= 1
CIRCUITPY_FREQUENCYIO ?= 1
CIRCUITPY_HASHLIB ?= 1
CIRCUITPY_I2CTARGET ?= 1
CIRCUITPY_IMAGECAPTURE = 0
CIRCUITPY_MEMORYMAP ?= 1
CIRCUITPY_NVM ?= 1
CIRCUITPY_PS2IO ?= 1
CIRCUITPY_RGBMATRIX ?= 1
CIRCUITPY_ROTARYIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE ?= 1
CIRCUITPY_WATCHDOG ?= 1
CIRCUITPY_WIFI ?= 1

# Conditionally turn off modules/features
ifeq ($(IDF_TARGET),esp32)
# Modules
CIRCUITPY_BLEIO = 0
CIRCUITPY_PARALLELDISPLAY = 0
CIRCUITPY_RGBMATRIX = 0
# Features
CIRCUITPY_USB = 0

else ifeq ($(IDF_TARGET),esp32c3)
# Modules
CIRCUITPY_ALARM = 0
CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_PARALLELDISPLAY = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_TOUCHIO ?= 1
CIRCUITPY_TOUCHIO_USE_NATIVE = 0
# Features
CIRCUITPY_USB = 0

else ifeq ($(IDF_TARGET),esp32s2)
# Modules
CIRCUITPY_BLEIO = 0
CIRCUITPY_RGBMATRIX_USES_SUPERVISOR_ALLOCATION = 0

else ifeq ($(IDF_TARGET),esp32s3)
# Modules
CIRCUITPY_PARALLELDISPLAY = 0
CIRCUITPY_RGBMATRIX_USES_SUPERVISOR_ALLOCATION = 0
endif

# No room for dualbank on boards with 2MB flash
ifeq ($(CIRCUITPY_ESP_FLASH_SIZE),2MB)
CIRCUITPY_DUALBANK = 0
endif

# Modules dependent on other modules
CIRCUITPY_ESPNOW ?= $(CIRCUITPY_WIFI)
CIRCUITPY_GIFIO ?= $(CIRCUITPY_ESPCAMERA)
CIRCUITPY_QRIO ?= $(CIRCUITPY_ESPCAMERA)

# Features dependent on other features
ifneq ($(CIRCUITPY_USB),0)
CIRCUITPY_BUILD_EXTENSIONS ?= bin,uf2
else
CIRCUITPY_BUILD_EXTENSIONS ?= bin
endif

# From ESP32-S2/S3 Technical Reference Manual:
#
# Endpoint number 0 always present (bi-directional, consisting of EP0 IN and EP0 OUT)
# Six additional endpoints (endpoint numbers 1 to 6), configurable as IN or OUT
# Maximum of five IN endpoints concurrently active at any time (including EP0 IN)
#
# Due to the limited number of endpoints, some USB devices will be off by default.
# For instance MIDI is available, but the device is turned off. It can be turned on
# only if something else is turned off, such as HID.
USB_NUM_ENDPOINT_PAIRS = 7
USB_NUM_IN_ENDPOINTS = 5
