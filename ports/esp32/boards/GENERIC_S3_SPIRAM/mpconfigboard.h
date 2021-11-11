#define MICROPY_HW_BOARD_NAME               "ESP32S3 module (spiram)"
#define MICROPY_HW_MCU_NAME                 "ESP32S3"

#define MICROPY_PY_BLUETOOTH                (0)
#define MICROPY_PY_MACHINE_DAC              (0)
#define MICROPY_HW_ESP32S3_EXTENDED_IO      (1)

#define MICROPY_HW_I2C0_SCL                 (9)
#define MICROPY_HW_I2C0_SDA                 (8)

#define MICROPY_HW_SPI1_MOSI                (35)  // SDO
#define MICROPY_HW_SPI1_MISO                (36)  // SDI
#define MICROPY_HW_SPI1_SCK                 (37)