// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME "Solder Party RP2350 Stamp XL"

#define MICROPY_HW_USB_VID (0x1209)
#define MICROPY_HW_USB_PID (0xA184)
#define MICROPY_HW_USB_MANUFACTURER_STRING "Solder Party"
#define MICROPY_HW_USB_PRODUCT_FS_STRING MICROPY_HW_BOARD_NAME " MicroPython"

// TODO: Test PSRAM option
#define MICROPY_HW_PSRAM_CS_PIN (8)
#define MICROPY_HW_ENABLE_PSRAM (0)
