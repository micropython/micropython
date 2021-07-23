// https://www.adafruit.com/product/4884
// https://learn.adafruit.com/adafruit-feather-rp2040-pico/pinouts

#define MICROPY_HW_BOARD_NAME          "Adafruit Feather RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x239A)
#define MICROPY_HW_USB_PID (0x80F2)

// STEMMA QT / Qwiic on I2C1
#define MICROPY_HW_I2C1_SCL  (3)
#define MICROPY_HW_I2C1_SDA  (2)

#define MICROPY_HW_SPI0_SCK  (18)
#define MICROPY_HW_SPI0_MOSI (19)
#define MICROPY_HW_SPI0_MISO (20)

// NeoPixel GPIO16, power not toggleable

// Red user LED GPIO13
