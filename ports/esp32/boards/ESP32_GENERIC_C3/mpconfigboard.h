// This configuration is for a generic ESP32C3 board with 4MiB (or more) of flash.

#define MICROPY_HW_BOARD_NAME               "ESP32C3 module"
#define MICROPY_HW_MCU_NAME                 "ESP32C3"

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)

/**
   22 Pins total
                          SDA      SCL
   For adafruit qtpy        5        6
   For xiao c3              6        7

   GPIO 8&9 are strapping (okay to use after start)
   GPIO 12-17 are typically used for SPI flash/PSRAM
   GPIO 18&19 are typically used for USB/JTAG

   GPIO 0-7,10-11,20-21 generally possible
   Without defining, SDL/SDA default is 19/18 which
   conflict with USB
**/
// define I2C pin defaults
#define MICROPY_HW_I2C0_SCL                 (7)
#define MICROPY_HW_I2C0_SDA                 (6)
