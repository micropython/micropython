// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME           "RP2040-Zero"
#define MICROPY_HW_FLASH_STORAGE_BYTES  (1536 * 1024)

// Enable USB Mass Storage with FatFS filesystem.
#define MICROPY_HW_USB_MSC (1)
