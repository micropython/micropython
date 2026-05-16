// Board configuration for the Dabao evaluation board (Crowd Supply).
// Pico-form-factor dev board around the Baochip-1x RISC-V SoC, designed
// by bunnie Huang.

#define MICROPY_HW_BOARD_NAME       "DABAO"
#define MICROPY_HW_MCU_NAME         "Baochip-1x"

// REPL routed to UART2 (PB14 TX / PB13 RX) by default.  These are the
// only UART pins broken out to the Dabao header.
#define MICROPY_HW_UART_REPL        (2)
#define MICROPY_HW_UART_REPL_BAUD   (115200)
