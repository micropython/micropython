#define MICROPY_HW_BOARD_NAME   "OpenMV-AE3"
#define MICROPY_HW_MCU_NAME     "AE302F80F55D5AE"

#define MICROPY_HW_USB_MSC      (0)

#define MICROPY_HW_ENABLE_OSPI  (0)
#define MICROPY_HW_ENABLE_UART_REPL (0)

#define MICROPY_HW_USB_VID      0x1209
#define MICROPY_HW_USB_PID      0xabd1

extern void board_startup(void);
#define MICROPY_BOARD_STARTUP   board_startup

// This is used for alif.Flash() and USB MSC.
#define MICROPY_HW_FLASH_STORAGE_BASE_ADDR      (0)
#define MICROPY_HW_FLASH_STORAGE_BYTES          (32 * 1024 * 1024)
