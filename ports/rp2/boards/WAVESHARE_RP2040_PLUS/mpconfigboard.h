// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Waveshare RP2040-Plus"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (3 * 1024 * 1024)

// Taken from https://github.com/raspberrypi/usb-pid
#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1020)
