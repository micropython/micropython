#define MICROPY_HW_BOARD_NAME               "LILYGO T3-S3"
#define MICROPY_HW_MCU_NAME                 "ESP32-S3"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-t3s3"

// I2C0 default pins — OLED SSD1306
// Verified: schematic T3S3_V1.2 U24 D0=SCL(IO17), D1=SDA(IO18) in I2C mode
#define MICROPY_HW_I2C0_SCL                 (17)
#define MICROPY_HW_I2C0_SDA                 (18)

// SPI1 default pins — SX1262 LoRa radio (HPD17 module)
// Verified: schematic T3S3_V1.2 HPD17 pin4=SCK(IO5), pin5=MOSI(IO6), pin6=MISO(IO3)
#define MICROPY_HW_SPI1_MOSI                (6)
#define MICROPY_HW_SPI1_MISO                (3)
#define MICROPY_HW_SPI1_SCK                 (5)
