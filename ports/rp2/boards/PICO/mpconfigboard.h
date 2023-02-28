// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1408 * 1024)

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x000c)

#define MICROPY_HW_USB_CDC                      (1)
#define MICROPY_HW_USB_MSC                      (1)
#define MICROPY_HW_USB_VENDOR                   (1)

#define MICROPY_PY_THREAD                       (1)