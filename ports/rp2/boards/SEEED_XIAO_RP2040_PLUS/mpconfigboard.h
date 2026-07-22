// https://wiki.seeedstudio.com/XIAO-RP2040-Plus/

#define MICROPY_HW_BOARD_NAME          "Seeed Studio XIAO RP2040 Plus"

// No VID/PID defined for the Seeed XIAO RP2040 Plus
// #define MICROPY_HW_USB_VID (0x)
// #define MICROPY_HW_USB_PID (0x)

// I2C0 (uses i2c1 hardware instance)
#define MICROPY_HW_I2C0_SCL  (7)
#define MICROPY_HW_I2C0_SDA  (6)

// I2C1 (uses i2c0 hardware instance, new on Plus)
#define MICROPY_HW_I2C1_SCL  (21)
#define MICROPY_HW_I2C1_SDA  (20)

// SPI0
#define MICROPY_HW_SPI0_SCK  (2)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (4)

// UART0 is, by default, assigned the correct pins (TX=0, RX=1)
