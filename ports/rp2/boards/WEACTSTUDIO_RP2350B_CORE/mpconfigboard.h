// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME "WeAct Studio RP2350B Core"
#define PICO_FLASH_SIZE_BYTES (16 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (2 * 1024 * 1024))

// TODO: Split PSRAM option off as a variant
#define MICROPY_HW_PSRAM_CS_PIN (0)
#define MICROPY_HW_ENABLE_PSRAM (0)