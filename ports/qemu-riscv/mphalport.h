#include <stddef.h>
#include "uart.h"

#define mp_hal_stdin_rx_chr() (0)
#define mp_hal_stdout_tx_strn_cooked(s, l) uart_tx_strn((s), (l))
