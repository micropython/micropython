// Board config for UNIT Electronics DualMCU RP2040 + ESP32.

// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME           "DualMCU RP2040"
#define MICROPY_HW_ENABLE_UART_REPL     (1)

// UART defaults from board pinout.
#define MICROPY_HW_UART0_TX             (0)
#define MICROPY_HW_UART0_RX             (1)
#define MICROPY_HW_UART0_CTS            (-1)
#define MICROPY_HW_UART0_RTS            (-1)

#define MICROPY_HW_UART1_TX             (8)
#define MICROPY_HW_UART1_RX             (9)
#define MICROPY_HW_UART1_CTS            (-1)
#define MICROPY_HW_UART1_RTS            (-1)

// I2C0 defaults from board pinout.
#define MICROPY_HW_I2C0_SDA             (12)
#define MICROPY_HW_I2C0_SCL             (13)

// SPI0 defaults from board pinout.
#define MICROPY_HW_SPI0_SCK             (18)
#define MICROPY_HW_SPI0_MOSI            (19)
#define MICROPY_HW_SPI0_MISO            (20)
