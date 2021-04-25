USB_VID = 0x239A
USB_PID = 0x801F
USB_PRODUCT = "Trinket M0"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21E18A
CHIP_FAMILY = samd21

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE
CIRCUITPY_FULL_BUILD = 0

SUPEROPT_GC = 0
SUPEROPT_VM = 0

# On this particular board, these save about 180 bytes. On other boards, they may -increase- space used.
CFLAGS_BOARD = -fweb -frename-registers
