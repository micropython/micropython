// Board and hardware specific configuration

#define MICROPY_HW_BOARD_NAME "SparkFun IoT RedBoard ESP32"
#define MICROPY_HW_MCU_NAME "ESP32"

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL     (1)

#define MICROPY_HW_I2C0_SCL            (22)
#define MICROPY_HW_I2C0_SDA            (21)

#define MICROPY_HW_SPI1_SCK  (18)
#define MICROPY_HW_SPI1_MOSI (23)
#define MICROPY_HW_SPI1_MISO (19)
