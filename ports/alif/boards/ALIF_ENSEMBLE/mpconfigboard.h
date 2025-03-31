#define MICROPY_HW_BOARD_NAME                   "Alif Ensemble DevKit"
#define MICROPY_HW_MCU_NAME                     "AE722F80F55D5XX"

#define MICROPY_HW_ENABLE_UART_REPL             (CORE_M55_HP)
#define MICROPY_HW_UART_REPL                    (4)
#define MICROPY_HW_USB_MSC                      (1)
#define MICROPY_HW_ENABLE_HW_I2C                (1)

// ROMFS partitions
#define MICROPY_HW_ROMFS_ENABLE_PART0           (1)
#define MICROPY_HW_ROMFS_ENABLE_PART1           (CORE_M55_HP)

// I2C buses
#define MICROPY_HW_I2C0_SCL                     (pin_P0_3)
#define MICROPY_HW_I2C0_SDA                     (pin_P0_2)
#define MICROPY_HW_I2C1_SCL                     (pin_P3_7)
#define MICROPY_HW_I2C1_SDA                     (pin_P3_6)
#define MICROPY_HW_I2C2_SCL                     (pin_P5_1)
#define MICROPY_HW_I2C2_SDA                     (pin_P5_0)
#define MICROPY_HW_I2C3_SCL                     (pin_P1_1)
#define MICROPY_HW_I2C3_SDA                     (pin_P1_0)

// SPI buses
#define MICROPY_HW_SPI0_MISO                    (pin_P1_0)
#define MICROPY_HW_SPI0_MOSI                    (pin_P1_1)
#define MICROPY_HW_SPI0_SCK                     (pin_P1_2)
#define MICROPY_HW_SPI1_MISO                    (pin_P2_4)
#define MICROPY_HW_SPI1_MOSI                    (pin_P2_5)
#define MICROPY_HW_SPI1_SCK                     (pin_P2_6)
#define MICROPY_HW_SPI2_MISO                    (pin_P4_2)
#define MICROPY_HW_SPI2_MOSI                    (pin_P4_3)
#define MICROPY_HW_SPI2_SCK                     (pin_P4_4)
#define MICROPY_HW_SPI3_MISO                    (pin_P12_4)
#define MICROPY_HW_SPI3_MOSI                    (pin_P12_5)
#define MICROPY_HW_SPI3_SCK                     (pin_P12_6)
#define MICROPY_HW_LPSPI0_MISO                  (pin_P7_4)
#define MICROPY_HW_LPSPI0_MOSI                  (pin_P7_5)
#define MICROPY_HW_LPSPI0_SCK                   (pin_P7_6)

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
