// Tanmatsu ESP32-P4 board configuration

#pragma once

#define MICROPY_HW_BOARD_NAME               "Tanmatsu"
#define MICROPY_HW_MCU_NAME                 "ESP32P4"


// Use USB-Serial-JTAG for REPL
#define MICROPY_HW_ENABLE_USB_SERIAL_JTAG (1)

// No UART console
#define MICROPY_HW_ENABLE_UART_REPL (0)

// No PSRAM
#define MICROPY_HW_ENABLE_SDCARD (0)
#define MICROPY_HW_ENABLE_SPIRAM (0)

// Internal flash
#define MICROPY_HW_FLASH_SIZE (16 * 1024 * 1024)

// Default pins (placeholder)
#define MICROPY_HW_I2C0_SCL (1)
#define MICROPY_HW_I2C0_SDA (2)
#define MICROPY_HW_SPI0_SCK (3)
#define MICROPY_HW_SPI0_MOSI (4)
#define MICROPY_HW_SPI0_MISO (5)

#define MICROPY_PY_NETWORK                  (0)

#define MICROPY_PY_BLUETOOTH                (0)
#define MICROPY_BLUETOOTH_NIMBLE            (0)
#define MICROPY_PY_MACHINE_I2C_TARGET       (0)
#define MICROPY_PY_ESPNOW                   (0)

#define MICROPY_BOARD_STARTUP                tanmatsu_board_startup
void tanmatsu_board_startup(void);
