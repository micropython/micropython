#define MICROPY_HW_BOARD_NAME       "Sparkfun SAMD51 Thing Plus"
#define MICROPY_HW_MCU_NAME         "SAMD51J20A"

#define MICROPY_HW_XOSC32K          (1)

// There seems to be an inconsistency in the SAMD51 Thing bootloader in that
// the bootloader magic address is at the end of a 192k RAM area, instead of
// 256k. Since the SAMD51x20A has 256k RAM, the loader symbol is at that address
// and so there is a fix here using the previous definition.
#define DBL_TAP_ADDR_ALT    ((volatile uint32_t *)(HSRAM_ADDR + HSRAM_SIZE - 0x10000 - 4))

// Enabling both two lines below will set the boot file system to
// the board's external flash.
#define MICROPY_HW_SPIFLASH         (1)
#define MICROPY_HW_SPIFLASH_ID      (0)

#define MICROPY_HW_WIFI_SPI_ID     (4)
#define MICROPY_HW_WIFI_SPI_BAUDRATE (8000000)

#define MICROPY_HW_WIFI_SPI_MOSI   (44)
#define MICROPY_HW_WIFI_SPI_MISO   (43)
#define MICROPY_HW_WIFI_SPI_SCK    (45)

#define MICROPY_HW_NINA_ACK        (16)
#define MICROPY_HW_NINA_CS         (17)
#define MICROPY_HW_NINA_RESET      (19)

// defines for BLE
#define MICROPY_PY_BLUETOOTH_NINAW10 (1)

#define MICROPY_HW_BLE_UART_ID       (2)
#define MICROPY_HW_BLE_UART_BAUDRATE (115200)

#define MICROPY_HW_BLE_UART_TX       (12)
#define MICROPY_HW_BLE_UART_RX       (13)

#define MICROPY_HW_NINA_RTS          (44)
#define MICROPY_HW_NINA_CTS          (16)
