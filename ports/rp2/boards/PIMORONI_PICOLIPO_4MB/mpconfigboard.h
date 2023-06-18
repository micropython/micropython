// https://shop.pimoroni.com/products/pimoroni-pico-lipo?variant=39386149093459

#define MICROPY_HW_BOARD_NAME          "Pimoroni Pico LiPo 4MB"
#define MICROPY_HW_FLASH_STORAGE_BYTES (3 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1002)

#define MICROPY_HW_UART1_TX  (8)
#define MICROPY_HW_UART1_RX  (9)
#define MICROPY_HW_UART1_CTS (10)
#define MICROPY_HW_UART1_RTS (11)

// User LED GPIO25

// VBUS_SENSE GPIO24

// BAT_SENSE GPIO29

// Boot button GPIO23
