#include "py/mphal.h"
#include "board.h"

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (Chip_UART_ReadLineStatus(CONSOLE_UART) & UART_LSR_RDR) {
            return (int) Chip_UART_ReadByte(CONSOLE_UART);
        }
    }
    return -1;
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    for (; len > 0; --len) {
        uint8_t ch = (uint8_t) *str++;
        while ((Chip_UART_ReadLineStatus(CONSOLE_UART) & UART_LSR_THRE) == 0) {}
        Chip_UART_SendByte(CONSOLE_UART, ch);
    }
}
