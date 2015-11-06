#include <string.h>
#include <unistd.h>
#include "py/mpconfig.h"
#include "py/mphal.h"

/*
 * Extra UART functions
 * These can be either optimized for a particular port, or reference
 * implementation below can be used.
 */

// Send "cooked" string of given length, where every occurance of
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
