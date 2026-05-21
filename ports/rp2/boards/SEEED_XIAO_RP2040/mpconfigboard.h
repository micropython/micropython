// https://wiki.seeedstudio.com/XIAO-RP2040/

#define MICROPY_HW_BOARD_NAME          "Seeed Studio XIAO RP2040"

// No VID/PID defined for the Seeed XIAO RP2040
// #define MICROPY_HW_USB_VID (0x)
// #define MICROPY_HW_USB_PID (0x)

// I2C0 is available on XIAO RP2040 Plus on GPIO20/GPIO21.
// These pins are not fitted on the standard XIAO RP2040.
#define MICROPY_HW_I2C0_SCL  (21)
#define MICROPY_HW_I2C0_SDA  (20)

// I2C1 is the default bus on GPIO6/GPIO7 (the standard XIAO RP2040 pins).
#define MICROPY_HW_I2C1_SCL  (7)
#define MICROPY_HW_I2C1_SDA  (6)

// SPI0
#define MICROPY_HW_SPI0_SCK  (2)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (4)

// UART0 is, by default, assigned the correct pins (TX=0, RX=1)
