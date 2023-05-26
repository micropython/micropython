// This configuration is for LUAT CORE ESP32C3 board with 4MiB (or more) of flash.

#define MICROPY_HW_BOARD_NAME               "LuatOS C3 CORE"
#define MICROPY_HW_MCU_NAME                 "ESP32C3"

#define MICROPY_HW_ENABLE_SDCARD            (0)
#define MICROPY_PY_MACHINE_DAC              (0)
#define MICROPY_PY_MACHINE_I2S              (0)

#define MICROPY_HW_I2C0_SCL                 (5)
#define MICROPY_HW_I2C0_SDA                 (4)

#define MICROPY_HW_SPI1_MOSI                (3)
#define MICROPY_HW_SPI1_MISO                (10)
#define MICROPY_HW_SPI1_SCK                 (2)
