// Board specific definitions for the SparkFun Thing Plus ESP32-C5.

#define MICROPY_HW_BOARD_NAME               "SparkFun Thing Plus ESP32-C5"
#define MICROPY_HW_MCU_NAME                 "ESP32-C5"

#define MICROPY_HW_I2C0_SCL            (24)
#define MICROPY_HW_I2C0_SDA            (23)

#define MICROPY_HW_SPI1_SCK  (10)
#define MICROPY_HW_SPI1_MOSI (8)
#define MICROPY_HW_SPI1_MISO (9)

// microSD card in SPI mode (shares the SPI1 bus): machine.SDCard(slot=2).
#define MICROPY_HW_SDCARD_SPI_MOSI (8)
#define MICROPY_HW_SDCARD_SPI_MISO (9)
#define MICROPY_HW_SDCARD_SPI_SCK  (10)
#define MICROPY_HW_SDCARD_SPI_CS   (25)
