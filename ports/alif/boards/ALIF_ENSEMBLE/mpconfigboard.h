#define MICROPY_HW_BOARD_NAME                   "Alif Ensemble DevKit"
#define MICROPY_HW_MCU_NAME                     "AE722F80F55D5XX"

#define MICROPY_HW_UART_REPL                    (4)
#define MICROPY_HW_USB_MSC                      (1)

// This is used for alif.Flash() and USB MSC.
#define MICROPY_HW_FLASH_STORAGE_BASE_ADDR      (0)
#define MICROPY_HW_FLASH_STORAGE_BYTES          (32 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_FS_BYTES       (32 * 1024 * 1024)
