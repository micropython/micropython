#include <string.h>
#include <unistd.h>
#include "py/mpconfig.h"
#include "pybstdio.h"

/*
 * Extra UART functions
 * These can be either optimized for a particular port, or reference,
 * not very optimal implementation below can be used.
 */

// Send "cooked" string of length, where every occurance of
// LF character is replaced with CR LF.
void stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    while (len--) {
        if (*str == '\n') {
            stdout_tx_strn("\r", 1);
        }
        stdout_tx_strn(str++, 1);
    }
}

// Send zero-terminated string
void stdout_tx_str(const char *str) {
    stdout_tx_strn(str, strlen(str));
}
