// https://www.sparkfun.com/products/17745

#define MICROPY_HW_BOARD_NAME          "SparkFun Thing Plus RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (15 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x1B4F)
#define MICROPY_HW_USB_PID (0x0025)

#define MICROPY_HW_I2C0_SCL  (17)
#define MICROPY_HW_I2C0_SDA  (16)

// MicroSD on SPI1 (non-default)
#define MICROPY_HW_SPI1_SCK  (14)
#define MICROPY_HW_SPI1_MOSI (15)
#define MICROPY_HW_SPI1_MISO (12)

// Battery fuel guage MAX17048 on I2C1
// BATT_ALERT GPIO24

// NeoPixel data GPIO8, power not toggleable
// data out is broken out to a pin

// Blue user LED GPIO25
