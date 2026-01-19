// https://www.adafruit.com/product/5056
// https://learn.adafruit.com/adafruit-trinkey-qt2040/pinouts

#define MICROPY_HW_BOARD_NAME          "Adafruit Trinkey QT2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x239A)
#define MICROPY_HW_USB_PID (0x8106)

// STEMMA QT / Qwiic on I2C0
#define MICROPY_HW_I2C0_SCL  (17)
#define MICROPY_HW_I2C0_SDA  (16)

// NeoPixel data GPIO27, power not toggleable

// Boot button GPIO12, can be used as user btn
