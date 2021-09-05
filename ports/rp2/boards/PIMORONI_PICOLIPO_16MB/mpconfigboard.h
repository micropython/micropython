// https://shop.pimoroni.com/products/pimoroni-pico-lipo?variant=39335427080275

#define MICROPY_HW_BOARD_NAME          "Pimoroni Pico LiPo 16MB"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1003)

#define MICROPY_HW_UART1_TX  (8)
#define MICROPY_HW_UART1_RX  (9)
#define MICROPY_HW_UART1_CTS (10)
#define MICROPY_HW_UART1_RTS (11)

// Qwiic on I2C0
#define MICROPY_HW_I2C0_SCL  (4)
#define MICROPY_HW_I2C0_SDA  (5)

#define MICROPY_HW_SPI0_SCK  (18)
#define MICROPY_HW_SPI0_MOSI (19)
#define MICROPY_HW_SPI0_MISO (16)

// User LED GPIO25

// VBUS_SENSE GPIO24

// BAT_SENSE GPIO29

// Boot button GPIO23
