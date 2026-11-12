// https://www.adafruit.com/product/5302
// https://learn.adafruit.com/adafruit-kb2040/pinouts

#define MICROPY_HW_BOARD_NAME          "Adafruit KB2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x239A)
#define MICROPY_HW_USB_PID (0x8106)

// Boot Button   GPIO11
// Neopixel data GPIO17
