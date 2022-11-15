// https://shop.pimoroni.com/products/tiny-2040

#define MICROPY_HW_BOARD_NAME          "Pimoroni Tiny 2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x16D0)
#define MICROPY_HW_USB_PID (0x08C7)

// I2C0 (non-default)
#define MICROPY_HW_I2C0_SCL  (4)
#define MICROPY_HW_I2C0_SDA  (5)

// RGB LED, active low
// Red LED 18
// Green LED 19
// Blue LED 20

// Boot button GPIO23
