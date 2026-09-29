#define MICROPY_HW_BOARD_NAME    "Waveshare ESP32-C6-Touch-LCD-1.47"
#define MICROPY_HW_MCU_NAME      "ESP32-C6"

// Onboard touch controller and IMU share this I2C bus.
#define MICROPY_HW_I2C0_SCL      (19)
#define MICROPY_HW_I2C0_SDA      (18)

// Onboard LCD and microSD card share this SPI bus.
#define MICROPY_HW_SPI1_MOSI     (2)
#define MICROPY_HW_SPI1_MISO     (3)
#define MICROPY_HW_SPI1_SCK      (1)
