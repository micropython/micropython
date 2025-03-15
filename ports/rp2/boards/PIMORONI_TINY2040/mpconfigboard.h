// https://shop.pimoroni.com/products/tiny-2040

#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME          "Pimoroni Tiny 2040 8MB"
#endif
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (1 * 1024 * 1024))

#define MICROPY_HW_USB_VID (0x16D0)
#define MICROPY_HW_USB_PID (0x08C7)

// I2C0 (non-default)
#define MICROPY_HW_I2C0_SCL  (4)
#define MICROPY_HW_I2C0_SDA  (5)
