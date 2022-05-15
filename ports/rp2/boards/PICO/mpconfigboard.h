// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
// Modified from MPY origin to reduce flash storage to accommodate larger program flash requirement
// of lvgl and its bindings. Developers should review this setting when adding additional features 
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1024 * 1024)

