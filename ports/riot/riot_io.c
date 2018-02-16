#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "py/mpconfig.h"

/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    int r = read(STDIN_FILENO, &c, 1);
    if (r) {
        if (c == '\n') {
            c = '\r';
        }
    }
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    int r = write(STDOUT_FILENO, str, len);
    (void)r;
}
/*
 * Extra IO functions
 */

// Send "cooked" string of length, where every occurance of
// LF character is replaced with CR LF.
void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    while (len--) {
        if (*str == '\n') {
            mp_hal_stdout_tx_strn("\r", 1);
        }
        mp_hal_stdout_tx_strn(str++, 1);
    }
}

// Send zero-terminated string
void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
