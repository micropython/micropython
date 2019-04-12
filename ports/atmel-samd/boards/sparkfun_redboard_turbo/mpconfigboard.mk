LD_FILE = boards/samd21x18-bootloader-external-flash.ld
USB_VID = 0x1B4F
USB_PID = 0x0015
USB_PRODUCT = "RedBoard Turbo Board"
USB_MANUFACTURER = "SparkFun Electronics"

SPI_FLASH_FILESYSTEM = 1
LONGINT_IMPL = MPZ

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "W25Q32FV"

# Tweak inlining depending on language.
ifeq ($(TRANSLATION), zh_Latn_pinyin)
CFLAGS_INLINE_LIMIT = 50
endif
