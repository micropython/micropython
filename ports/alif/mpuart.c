/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mphal.h"
#include "py/runtime.h"
#include "mpuart.h"

#include "sys_ctrl_uart.h"
#include "uart.h"

#define UART_MAX (8)
#define SYST_PCLK (100000000)

typedef struct _uart_state_t {
    UART_TRANSFER_STATUS status;
    ringbuf_t *rx_ringbuf;
    const uint8_t *tx_src;
    const uint8_t *tx_src_max;
    void (*irq_callback)(void);
} uart_state_t;

static const uint8_t uart_irqn[UART_MAX] = {
    UART0_IRQ_IRQn,
    UART1_IRQ_IRQn,
    UART2_IRQ_IRQn,
    UART3_IRQ_IRQn,
    UART4_IRQ_IRQn,
    UART5_IRQ_IRQn,
    UART6_IRQ_IRQn,
    UART7_IRQ_IRQn,
};

static UART_Type *const uart_periph[UART_MAX] = {
    (UART_Type *)UART0_BASE,
    (UART_Type *)UART1_BASE,
    (UART_Type *)UART2_BASE,
    (UART_Type *)UART3_BASE,
    (UART_Type *)UART4_BASE,
    (UART_Type *)UART5_BASE,
    (UART_Type *)UART6_BASE,
    (UART_Type *)UART7_BASE,
};

static uart_state_t uart_state[UART_MAX];

void mp_uart_init(unsigned int uart_id, uint32_t baudrate, mp_hal_pin_obj_t tx, mp_hal_pin_obj_t rx, ringbuf_t *rx_ringbuf) {
    UART_Type *uart = uart_periph[uart_id];
    uart_state_t *state = &uart_state[uart_id];

    // Configure TX/RX pins.
    mp_hal_pin_config(tx, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT_UART, false);
    mp_hal_pin_config(rx, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT_UART, true);

    // Configure the UART peripheral.
    select_uart_clock_syst_pclk(uart_id);
    enable_uart_clock(uart_id);
    uart_software_reset(uart);
    uart_enable_fifo(uart);
    uart_disable_tx_irq(uart);
    uart_disable_rx_irq(uart);
    uart_set_baudrate(uart, SYST_PCLK, baudrate);
    uart_set_data_parity_stop_bits(uart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
    uart_set_flow_control(uart, UART_FLOW_CONTROL_NONE);
    uart->UART_FCR |= UART_FCR_RCVR_FIFO_RESET;
    uart_set_tx_trigger(uart, UART_TX_FIFO_EMPTY);
    uart_set_rx_trigger(uart, UART_RX_ONE_CHAR_IN_FIFO);

    // Initialise the state.
    state->status = UART_TRANSFER_STATUS_NONE;
    state->rx_ringbuf = rx_ringbuf;
    state->tx_src = NULL;
    state->tx_src_max = NULL;
    state->irq_callback = NULL;

    // Enable interrupts.
    NVIC_ClearPendingIRQ(uart_irqn[uart_id]);
    NVIC_SetPriority(uart_irqn[uart_id], IRQ_PRI_UART_REPL);
    NVIC_EnableIRQ(uart_irqn[uart_id]);
    uart_enable_rx_irq(uart);
}

void mp_uart_deinit(unsigned int uart_id) {
    UART_Type *uart = uart_periph[uart_id];

    uart_disable_rx_irq(uart);
    NVIC_DisableIRQ(uart_irqn[uart_id]);
}

void mp_uart_set_irq_callback(unsigned int uart_id, void (*callback)(void)) {
    uart_state_t *state = &uart_state[uart_id];
    state->irq_callback = callback;
}

void mp_uart_set_flow(unsigned int uart_id, mp_hal_pin_obj_t rts, mp_hal_pin_obj_t cts) {
    UART_Type *uart = uart_periph[uart_id];

    unsigned int flow = UART_FLOW_CONTROL_NONE;
    if (rts != NULL) {
        flow |= UART_FLOW_CONTROL_RTS;
        mp_hal_pin_config(rts, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT_UART, false);
    }
    if (cts != NULL) {
        flow |= UART_FLOW_CONTROL_CTS;
        mp_hal_pin_config(cts, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT_UART, true);
    }
    uart_set_flow_control(uart, flow);
}

void mp_uart_set_baudrate(unsigned int uart_id, uint32_t baudrate) {
    UART_Type *uart = uart_periph[uart_id];

    uart_set_baudrate(uart, SYST_PCLK, baudrate);
}

size_t mp_uart_rx_any(unsigned int uart_id) {
    uart_state_t *state = &uart_state[uart_id];
    if (state->rx_ringbuf != NULL) {
        return ringbuf_avail(state->rx_ringbuf);
    }
    return 0;
}

int mp_uart_rx_char(unsigned int uart_id) {
    uart_state_t *state = &uart_state[uart_id];
    if (state->rx_ringbuf != NULL && ringbuf_avail(state->rx_ringbuf)) {
        return ringbuf_get(state->rx_ringbuf);
    }
    return -1;
}

void mp_uart_tx_data_blocking(unsigned int uart_id, const uint8_t *src, size_t len) {
    UART_Type *uart = uart_periph[uart_id];

    for (size_t i = 0; i < len; ++i) {
        for (;;) {
            size_t tx_avail = UART_FIFO_DEPTH - uart->UART_TFL;
            if (tx_avail > 0) {
                break;
            }
            mp_event_handle_nowait();
        }
        uart->UART_THR = src[i];
    }
}

void mp_uart_tx_data(unsigned int uart_id, const uint8_t *src, size_t len) {
    UART_Type *uart = uart_periph[uart_id];

    // Configure transmission, to be handled by IRQ.
    uart_state_t *state = &uart_state[uart_id];
    state->status = UART_TRANSFER_STATUS_NONE;
    state->tx_src = src;
    state->tx_src_max = src + len;

    // Enable TX IRQ to start transmission.
    uart_enable_tx_irq(uart);

    // Wait for transfer to complete.
    const uint32_t total_timeout_ms = 100 * len;
    uint32_t start = mp_hal_ticks_ms();
    while (state->status == UART_TRANSFER_STATUS_NONE) {
        if (mp_hal_ticks_ms() - start > total_timeout_ms) {
            break;
        }
        mp_event_wait_indefinite();
    }

    // Disable TX IRQ.
    uart_disable_tx_irq(uart);
}

static void mp_uart_irq_handler(unsigned int uart_id) {
    UART_Type *uart = uart_periph[uart_id];
    uart_state_t *state = &uart_state[uart_id];

    // Process pending interrupt (below is order of highest to lowest priority).
    uint32_t iir = uart->UART_IIR & UART_IIR_INTERRUPT_ID_MASK;
    switch (iir) {
        case UART_IIR_RECEIVER_LINE_STATUS: {
            uint32_t lsr = uart->UART_LSR;
            if (lsr & (UART_LSR_RECEIVER_FIFO_ERR | UART_LSR_OVERRUN_ERR)) {
                state->status = UART_TRANSFER_STATUS_ERROR;
            }
            break;
        }

        case UART_IIR_RECEIVED_DATA_AVAILABLE:
        case UART_IIR_CHARACTER_TIMEOUT:
            while (uart->UART_USR & UART_USR_RECEIVE_FIFO_NOT_EMPTY) {
                for (uint32_t rfl = uart->UART_RFL; rfl; --rfl) {
                    int c = uart->UART_RBR;
                    #if MICROPY_HW_ENABLE_UART_REPL && MICROPY_KBD_EXCEPTION
                    if (uart_id == MICROPY_HW_UART_REPL) {
                        if (c == mp_interrupt_char) {
                            mp_sched_keyboard_interrupt();
                            continue;
                        }
                    }
                    #endif
                    if (state->rx_ringbuf != NULL) {
                        ringbuf_put(state->rx_ringbuf, c);
                    }
                }
            }

            if (iir == UART_IIR_CHARACTER_TIMEOUT) {
                if (state->irq_callback != NULL) {
                    state->irq_callback();
                }
            }

            break;

        case UART_IIR_TRANSMIT_HOLDING_REG_EMPTY:
            while (uart->UART_USR & UART_USR_TRANSMIT_FIFO_NOT_FULL) {
                if (state->tx_src < state->tx_src_max) {
                    uart->UART_THR = *state->tx_src++;
                } else {
                    uart_disable_tx_irq(uart);
                    state->status = UART_TRANSFER_STATUS_SEND_COMPLETE;
                    break;
                }
            }
            break;

        case UART_IIR_MODEM_STATUS:
            (void)uart->UART_MSR;
            break;
    }

    __SEV();
}

#define DEFINE_IRQ_HANDLER(id) \
    void UART##id##_IRQHandler(void) { \
        mp_uart_irq_handler(id); \
    }

DEFINE_IRQ_HANDLER(0)
DEFINE_IRQ_HANDLER(1)
DEFINE_IRQ_HANDLER(2)
DEFINE_IRQ_HANDLER(3)
DEFINE_IRQ_HANDLER(4)
DEFINE_IRQ_HANDLER(5)
DEFINE_IRQ_HANDLER(6)
DEFINE_IRQ_HANDLER(7)

#if MICROPY_HW_ENABLE_UART_REPL

#define REPL_BAUDRATE (115200)

void mp_uart_init_repl(void) {
    mp_uart_init(MICROPY_HW_UART_REPL, REPL_BAUDRATE, pin_REPL_UART_TX, pin_REPL_UART_RX, &stdin_ringbuf);
}

void mp_uart_write_strn_repl(const char *str, size_t len) {
    mp_uart_tx_data(MICROPY_HW_UART_REPL, (const uint8_t *)str, len);
}

#endif
