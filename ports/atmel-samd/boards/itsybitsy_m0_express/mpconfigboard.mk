LD_FILE = boards/samd21x18-bootloader-external-flash-crystalless.ld
USB_VID = 0x239A
USB_PID = 0x8012
USB_PRODUCT = "ItsyBitsy M0 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 2
EXTERNAL_FLASH_DEVICES = "W25Q16FW, GD25Q16C"
LONGINT_IMPL = MPZ

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

# Tweak inlining depending on language.
ifeq ($(TRANSLATION), zh_Latn_pinyin)
CFLAGS_INLINE_LIMIT = 45
else
CFLAGS_INLINE_LIMIT = 70
endif
