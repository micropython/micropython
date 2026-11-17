#define MICROPY_HW_BOARD_NAME               "Waveshare ESP32-S3-Pico"
#define MICROPY_HW_MCU_NAME                 "ESP32-S3"

#define MICROPY_HW_I2C0_SCL                 (7)
#define MICROPY_HW_I2C0_SDA                 (6)

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)
