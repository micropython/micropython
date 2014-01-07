#include "misc.h"
#include "../stm/usart.h"

void usart_init(void) {
}

bool usart_is_enabled(void) {
    return false;
}

bool usart_rx_any(void) {
    return false;
}

int usart_rx_char(void) {
    return 0;
}

void usart_tx_char(int c) {
    (void)c;
}

void usart_tx_str(const char *str) {
    (void)str;
}

void usart_tx_strn_cooked(const char *str, int len) {
    (void)str;
    (void)len;
}
