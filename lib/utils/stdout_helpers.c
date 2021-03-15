#include <string.h>
#include <unistd.h>
#include "py/mpconfig.h"
#include "py/mphal.h"

/*
 * Extra stdout functions
 * These can be either optimized for a particular port, or reference
 * implementation below can be used.
 */

// Send "cooked" string of given length, where every occurrence of
// LF character is replaced with CR LF.
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    bool last_cr = false;
    while (len > 0) {
        size_t i = 0;
        if (str[0] == '\n' && !last_cr) {
            mp_hal_stdout_tx_strn("\r", 1);
            i = 1;
        }
        // Lump all characters on the next line together.
        while ((last_cr || str[i] != '\n') && i < len) {
            last_cr = str[i] == '\r';
            i++;
        }
        mp_hal_stdout_tx_strn(str, i);
        str = &str[i];
        len -= i;
    }
}

// Send zero-terminated string
void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
