// Board and hardware specific configuration
#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
#endif

#define MICROPY_HW_FLASH_STORAGE_BYTES          (1408 * 1024)

// partial typing implementation in C, disabled by default
#ifndef MICROPY_PY_TYPING
#define MICROPY_PY_TYPING           (0)
#define MICROPY_PY_TYPING_EXTRA_MODULES (0)
#endif
