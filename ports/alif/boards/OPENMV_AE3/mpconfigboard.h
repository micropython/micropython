#define MICROPY_HW_BOARD_NAME                   "OpenMV AE3"
#define MICROPY_HW_MCU_NAME                     "AE722F80F55D5XX"

#define MICROPY_HW_ENABLE_UART_REPL             (0)
#define MICROPY_HW_ENABLE_OSPI                  (0)

// This is used for alif.Flash() and USB MSC.
#define MICROPY_HW_FLASH_STORAGE_BASE_ADDR      (0)
#define MICROPY_HW_FLASH_STORAGE_BYTES          (32 * 1024 * 1024)

#define MICROPY_BOARD_STARTUP()                 board_startup()

void board_startup(void);
