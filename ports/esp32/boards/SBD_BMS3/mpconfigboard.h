#define MICROPY_HW_BOARD_NAME               "Bee S3"
#define MICROPY_HW_MCU_NAME                 "ESP32S3"

#define MICROPY_PY_MACHINE_DAC              (0)

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)

#define MICROPY_HW_I2C0_SCL                 (36)
#define MICROPY_HW_I2C0_SDA                 (37)

#define MICROPY_HW_SPI1_MOSI                (15)
#define MICROPY_HW_SPI1_MISO                (16)
#define MICROPY_HW_SPI1_SCK                 (17)
