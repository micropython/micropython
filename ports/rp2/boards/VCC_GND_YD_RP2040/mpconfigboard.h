#define MICROPY_HW_BOARD_NAME "VCC-GND Studio YD-RP2040"

// Allow 1MB for the firmware image itself, allocate the remainder to the filesystem
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (1 * 1024 * 1024))
