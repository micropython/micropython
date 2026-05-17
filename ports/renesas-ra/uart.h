/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#ifndef MICROPY_INCLUDED_RA_UART_H
#define MICROPY_INCLUDED_RA_UART_H

#include "shared/runtime/mpirq.h"
#include "shared/runtime/softtimer.h"
#include "pin.h"

typedef enum {
    HW_UART_0 = 0,
    HW_UART_1 = 1,
    HW_UART_2 = 2,
    HW_UART_3 = 3,
    HW_UART_4 = 4,
    HW_UART_5 = 5,
    HW_UART_6 = 6,
    HW_UART_7 = 7,
    HW_UART_8 = 8,
    HW_UART_9 = 9,
} machine_uart_t;

#define CHAR_WIDTH_8BIT (0)
#define CHAR_WIDTH_9BIT (1)

#define UART_WORDLENGTH_8B  (8)
#define UART_STOPBITS_1     (1)
#define UART_STOPBITS_2     (2)
#define UART_PARITY_NONE    (0)
#define UART_PARITY_ODD     (1)
#define UART_PARITY_EVEN    (2)

#define UART_HWCONTROL_CTS  (1)
#define UART_HWCONTROL_RTS  (2)

#define UART_IRQ_RX (0x10)
#define UART_IRQ_RXIDLE (0x1000)
#define RXIDLE_TIMER_MIN (1)

// OR-ed IRQ flags which are allowed to be used by the user
#define MP_UART_ALLOWED_FLAGS ((uint32_t)(UART_IRQ_RX | UART_IRQ_RXIDLE))

// OR-ed IRQ flags which should not be touched by the user
#define MP_UART_RESERVED_FLAGS ((uint16_t)0x0020)

enum {
    RXIDLE_INACTIVE,
    RXIDLE_STANDBY,
    RXIDLE_ARMED,
    RXIDLE_ALERT,
};

typedef struct _soft_timer_entry_extended_t {
    soft_timer_entry_t base;
    void *context;
} soft_timer_entry_extended_t;

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    machine_uart_t uart_id : 8;
    uint32_t baudrate;
    const machine_pin_obj_t *rx;
    const machine_pin_obj_t *tx;
    const machine_pin_obj_t *cts;
    const machine_pin_obj_t *rts;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint8_t flow;
    bool is_static : 1;
    bool is_enabled : 1;
    bool attached_to_repl;              // whether the UART is attached to REPL
    byte char_width;                    // 0 for 7,8 bit chars, 1 for 9 bit chars
    uint16_t char_mask;                 // 0x7f for 7 bit, 0xff for 8 bit, 0x1ff for 9 bit
    uint16_t timeout;                   // timeout waiting for first char
    uint16_t timeout_char;              // timeout waiting between chars
    uint16_t read_buf_len;              // len in chars; buf can hold len-1 chars
    byte *read_buf;                     // byte or uint16_t, depending on char size
    uint16_t mp_irq_trigger;            // user IRQ trigger mask
    uint16_t mp_irq_flags;              // user IRQ active IRQ flags
    mp_irq_obj_t *mp_irq_obj;           // user IRQ object
    soft_timer_entry_extended_t rxidle_timer;
    uint8_t rxidle_state;
    uint16_t rxidle_ms;
} machine_uart_obj_t;

extern const mp_irq_methods_t uart_irq_methods;

void uart_init0(void);
void uart_deinit_all(void);
bool uart_exists(int uart_id);
bool uart_init(machine_uart_obj_t *uart_obj,
    uint32_t baudrate, uint32_t bits, uint32_t parity, uint32_t stop, uint32_t flow);
void uart_irq_config(machine_uart_obj_t *self, bool enable);
void uart_set_rxbuf(machine_uart_obj_t *self, size_t len, void *buf);
void uart_deinit(machine_uart_obj_t *uart_obj);
// void uart_irq_handler(mp_uint_t uart_id);
void uart_irq_configure_timer(machine_uart_obj_t *self, mp_uint_t trigger);
void uart_soft_timer_callback(soft_timer_entry_t *self);

void uart_attach_to_repl(machine_uart_obj_t *self, bool attached);
uint32_t uart_get_baudrate(machine_uart_obj_t *self);
mp_uint_t uart_rx_any(machine_uart_obj_t *uart_obj);
mp_uint_t uart_tx_avail(machine_uart_obj_t *uart_obj);
mp_uint_t uart_tx_busy(machine_uart_obj_t *uart_obj);
mp_uint_t uart_tx_txbuf(machine_uart_obj_t *self);
bool uart_rx_wait(machine_uart_obj_t *self, uint32_t timeout);
int uart_rx_char(machine_uart_obj_t *uart_obj);
bool uart_tx_wait(machine_uart_obj_t *self, uint32_t timeout);
size_t uart_tx_data(machine_uart_obj_t *self, const void *src_in, size_t num_chars, int *errcode);
void uart_tx_strn(machine_uart_obj_t *uart_obj, const char *str, uint len);

#endif // MICROPY_INCLUDED_RA_UART_H
