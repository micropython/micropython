USB_VID = 0x239A
USB_PID = 0x8036
USB_PRODUCT = "PyPortal"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD51J20A
CHIP_FAMILY = samd51

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "W25Q64JVxQ, GD25Q64C"
LONGINT_IMPL = MPZ

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ConnectionManager
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_PortalBase
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ESP32SPI
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_FakeRequests

# The ESP32 co-processor, shared by the core and ESP32SPI. The core uses it
# for web workflow. The flags are set one by one, because CIRCUITPY_NETWORK
# would also turn on ssl, ipaddress and so on.
CIRCUITPY_AIRLIFT = 1
CIRCUITPY_SOCKETPOOL = 1
CIRCUITPY_WEB_WORKFLOW = 1
CIRCUITPY_MDNS = 0
CIRCUITPY_SSL = 0
