// This configuration is for a generic ESP32C3 board with 4MiB (or more) of flash.

#define MICROPY_HW_BOARD_NAME               "ZenSpecs"
#define MICROPY_HW_MCU_NAME                 "c3"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "serenspecs"

#define MICROPY_HW_ENABLE_SDCARD            (1)
#define MICROPY_PY_MACHINE_I2S              (1)
#define MICROPY_VFS_LITTLEFS                (1)


// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)
