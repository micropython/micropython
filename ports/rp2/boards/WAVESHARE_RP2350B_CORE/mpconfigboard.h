// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME "Waveshare RP2350B Core"
#define PICO_FLASH_SIZE_BYTES (16 * 1024 * 1024)
#define MICROPY_HW_FLASH_STORAGE_BYTES (14 * 1024 * 1024)

#define MICROPY_HW_PSRAM_CS_PIN (47)
#define MICROPY_HW_ENABLE_PSRAM (1)

// Override machine_uart.c definitions.
// See waveshare_rp2350b.h and note that the PICO_DEFAULT_UART configuration
// is not currently referenced in machine_uart.c.
#define MICROPY_HW_UART0_TX     (0)
#define MICROPY_HW_UART0_RX     (1)
#define MICROPY_HW_UART0_CTS    (-1)
#define MICROPY_HW_UART0_RTS    (-1)
