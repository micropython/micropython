#ifndef MICROPY_INCLUDED_TEENSY_UART_H
#define MICROPY_INCLUDED_TEENSY_UART_H

typedef enum {
    PYB_UART_NONE = 0,
} pyb_uart_t;

typedef struct _pyb_uart_obj_t pyb_uart_obj_t;

extern const mp_obj_type_t pyb_uart_type;

mp_uint_t uart_rx_any(pyb_uart_obj_t *uart_obj);
int uart_rx_char(pyb_uart_obj_t *uart_obj);
void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len);

#endif // MICROPY_INCLUDED_TEENSY_UART_H
