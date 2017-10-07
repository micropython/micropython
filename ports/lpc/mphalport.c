#include "py/mphal.h"
#include "board.h"

int mp_hal_stdin_rx_chr(void) {
    int c;
    do {
        c = Board_UARTGetChar();
    } while (c == EOF);
    return c;
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    for (; len > 0; --len) {
        Board_UARTPutChar(*str++);
    }
}

