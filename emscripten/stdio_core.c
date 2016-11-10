#include <unistd.h>
#include "stdio.h"
#include "py/mpconfig.h"

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = fgetc(stdin);
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    fwrite(str, len, 1, stdout); 
}
