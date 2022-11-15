// https://www.sparkfun.com/products/17717

#define MICROPY_HW_BOARD_NAME          "SparkFun Pro Micro RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (15 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x1B4F)
#define MICROPY_HW_USB_PID (0x0026)

#define MICROPY_HW_UART1_TX  (8)
#define MICROPY_HW_UART1_RX  (9)
#define MICROPY_HW_UART1_CTS (10)
#define MICROPY_HW_UART1_RTS (11)

// NeoPixel data GPIO25, power not toggleable
