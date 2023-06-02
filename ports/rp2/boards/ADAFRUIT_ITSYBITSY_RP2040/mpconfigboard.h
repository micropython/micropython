// https://www.adafruit.com/product/4888
// https://learn.adafruit.com/adafruit-itsybitsy-rp2040/pinouts

#define MICROPY_HW_BOARD_NAME          "Adafruit ItsyBitsy RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x239A)
#define MICROPY_HW_USB_PID (0x80FE)

// NeoPixel data GPIO17, power GPIO16

// Red user LED GPIO11

// Boot button GPIO13
