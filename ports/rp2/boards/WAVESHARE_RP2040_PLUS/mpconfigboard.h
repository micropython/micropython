#define MICROPY_HW_BOARD_NAME "Waveshare RP2040-Plus"

// Allow 1MB for the firmware image itself, allocate the remainder to the filesystem
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (640 * 1024))
