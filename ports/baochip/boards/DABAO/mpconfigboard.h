// Board configuration for the Dabao evaluation board (Crowd Supply).
// Pico-form-factor dev board around the Baochip-1x RISC-V SoC, designed
// by bunnie Huang.

#define MICROPY_HW_BOARD_NAME       "DABAO"
#define MICROPY_HW_MCU_NAME         "Baochip-1x"

// REPL routed to UART2 (PB14 TX / PB13 RX) by default.  These are the
// only UART pins broken out to the Dabao header.
#define MICROPY_HW_REPL_UART        (2)
#define MICROPY_HW_REPL_BAUDRATE    (115200)

// PC13 is the PROG strap (SW2) used to signal boot1 on reset.
// It is also the 0.85V regulator feedback pin -- hold LOW only briefly.
#define MICROPY_HW_BOOTLOADER_PORT  (2)
#define MICROPY_HW_BOOTLOADER_PIN   (13)
