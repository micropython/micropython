#ifndef MICROPY_INCLUDED_PIC32_UART_H
#define MICROPY_INCLUDED_PIC32_UART_H

#include <stdint.h>

typedef enum
{
    PIC32_UART_1,
    PIC32_UART_2,
    PIC32_UART_3,
    PIC32_UART_4,
    PIC32_UART_5,
    PIC32_UART_6,
}pic32_uart_t;

typedef enum
{
    NO_PARITY_8_BIT_DATA   = (0 << 1),
    EVEN_PARITY_8_BIT_DATA = (1 << 1),
    ODD_PARITY_8_BIT_DATA  = (2 << 1),
    NO_PARITY_9_BIT_DATA   = (3 << 1),
}pic32_uart_parity_data_t;

typedef enum
{
    ONE_STOP_BIT  = 0,
    TWO_STOP_BITS = 1,
}pic32_uart_stop_bit_t;

typedef enum
{
    IF_RBUF_NOT_EMPTY        = (0 << 6),
    IF_RBUF_FULL_1_2_OR_MORE = (1 << 6),
    IF_RBUF_FULL_3_4_OR_MORE = (2 << 6),
    IF_RBUF_RESERVED         = (3 << 6),
}pic32_uart_rx_interrupt_t;

typedef enum
{
    IF_TBUF_AT_LEAST_ONE   = (0 << 14),
    IF_TBUF_ALL_TRANSMITED = (1 << 14),
    IF_TBUF_IS_EMPTY       = (2 << 14),
    IF_TBUF_RESERVED       = (3 << 14),
}pic32_uart_tx_interrupt_t;

int uart_rx_ready(pic32_uart_t uart_id);

int uart_read_char(pic32_uart_t uart_id);

void uart_send_char(pic32_uart_t uart_id, int c);

void uart_init(pic32_uart_t uart_id,
    pic32_uart_parity_data_t pdata,
    pic32_uart_stop_bit_t stop_bit,
    uint32_t baud);

#endif // MICROPY_INCLUDED_PIC32_UART_H
