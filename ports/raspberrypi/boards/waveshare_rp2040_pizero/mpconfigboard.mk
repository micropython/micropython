USB_VID = 0x2E8A
USB_PID = 0x1083
USB_PRODUCT = "RP2040-PiZero"
USB_MANUFACTURER = "Waveshare Electronics"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q128JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_PICODVI = 1
# Disable native USB host because it won't work alongside DVI anyway. (They both
# use the second core.)
CIRCUITPY_USB_HOST = 0
