#ifndef MICROPY_INCLUDED_EXTMOD_NIMBLE_HAL_HAL_UART_H
#define MICROPY_INCLUDED_EXTMOD_NIMBLE_HAL_HAL_UART_H

#include <stdint.h>

#define SYSINIT_PANIC_ASSERT_MSG(cond, msg)

#define HAL_UART_PARITY_NONE (0)

typedef int (*hal_uart_tx_cb_t)(void *arg);
typedef int (*hal_uart_rx_cb_t)(void *arg, uint8_t data);

// Called by NimBLE, implemented in hal_uart.c.
int hal_uart_init_cbs(uint32_t port, hal_uart_tx_cb_t tx_cb, void *tx_arg, hal_uart_rx_cb_t rx_cb, void *rx_arg);
int hal_uart_config(uint32_t port, uint32_t baud, uint32_t bits, uint32_t stop, uint32_t parity, uint32_t flow);
void hal_uart_start_tx(uint32_t port);
int hal_uart_close(uint32_t port);

#endif // MICROPY_INCLUDED_EXTMOD_NIMBLE_HAL_HAL_UART_H
