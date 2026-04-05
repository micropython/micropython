// https://wiki.seeedstudio.com/XIAO-RP2040/

#define MICROPY_HW_BOARD_NAME          "Seeed Studio XIAO RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (1408 * 1024)

// No VID/PID defined for the Seeed XIAO RP2040
// #define MICROPY_HW_USB_VID (0x)
// #define MICROPY_HW_USB_PID (0x)

// I2C0
#define MICROPY_HW_I2C0_SCL  (7)
#define MICROPY_HW_I2C0_SDA  (6)

// SPI0
#define MICROPY_HW_SPI0_SCK  (2)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (4)

// UART0 is, by default, assigned the correct pins (TX=0, RX=1)
