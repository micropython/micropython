#define MICROPY_HW_BOARD_NAME                   "Alif Ensemble DevKit"
#define MICROPY_HW_MCU_NAME                     "AE722F80F55D5XX"

#define MICROPY_HW_ENABLE_UART_REPL             (CORE_M55_HP)
#define MICROPY_HW_UART_REPL                    (4)
#define MICROPY_HW_USB_MSC                      (1)

// ROMFS partitions
#define MICROPY_HW_ROMFS_ENABLE_PART0           (1)
#define MICROPY_HW_ROMFS_ENABLE_PART1           (CORE_M55_HP)

// UART buses
#define MICROPY_HW_UART0_TX                     (pin_P0_1)
#define MICROPY_HW_UART0_RX                     (pin_P0_0)
#define MICROPY_HW_UART0_RTS                    (pin_P0_3)
#define MICROPY_HW_UART0_CTS                    (pin_P0_2)
#define MICROPY_HW_UART1_TX                     (pin_P0_5)
#define MICROPY_HW_UART1_RX                     (pin_P0_4)
#define MICROPY_HW_UART1_RTS                    (pin_P0_7)
#define MICROPY_HW_UART1_CTS                    (pin_P0_6)
#define MICROPY_HW_REPL_UART_TX                 (pin_P12_2)
#define MICROPY_HW_REPL_UART_RX                 (pin_P12_1)

// This is used for alif.Flash() and USB MSC.
#define MICROPY_HW_FLASH_STORAGE_BASE_ADDR      (0)
#define MICROPY_HW_FLASH_STORAGE_BYTES          (32 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_FS_BYTES       (16 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_ROMFS_BYTES    (16 * 1024 * 1024)
