USB_VID = 0x239A
USB_PID = 0x8052
USB_PRODUCT = "ItsyBitsy nRF52840 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

MCU_CHIP = nrf52840

# Don't use up a hardware SPI peripheral for the status DotStar: we only have one or two.
CIRCUITPY_BITBANG_APA102 = 1

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "GD25Q16C"
