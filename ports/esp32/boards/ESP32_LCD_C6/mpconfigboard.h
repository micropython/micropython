// This configuration is for a generic ESP32C6 board with 4MiB (or more) of flash.

#define MICROPY_HW_BOARD_NAME               "ESP32C6 module"
#define MICROPY_HW_MCU_NAME                 "ESP32C6"

#define MICROPY_HW_ENABLE_SDCARD            (0)
#define MICROPY_PY_MACHINE_I2S              (0)

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)
