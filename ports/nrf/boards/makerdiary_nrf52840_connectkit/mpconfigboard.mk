USB_VID = 0x2886
USB_PID = 0xF003
USB_PRODUCT = "Makerdiary nRF52840 Connect Kit"
USB_MANUFACTURER = "Makerdiary"
SOFTDEV_VERSION=7.0.1

MCU_CHIP = nrf52840

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "MX25R6435F"

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BLE
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
