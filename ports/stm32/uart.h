/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_UART_H
#define MICROPY_INCLUDED_STM32_UART_H

#include "shared/runtime/mpirq.h"

typedef enum {
    PYB_UART_NONE = 0,
    PYB_UART_1 = 1,
    PYB_UART_2 = 2,
    PYB_UART_3 = 3,
    PYB_UART_4 = 4,
    PYB_UART_5 = 5,
    PYB_UART_6 = 6,
    PYB_UART_7 = 7,
    PYB_UART_8 = 8,
    PYB_UART_9 = 9,
    PYB_UART_10 = 10,
    #ifdef LPUART1
    PYB_LPUART_1 = MICROPY_HW_MAX_UART + 1,
    #endif
    #ifdef LPUART2
    PYB_LPUART_2 = MICROPY_HW_MAX_UART + 2,
    #endif
} pyb_uart_t;

#define CHAR_WIDTH_8BIT (0)
#define CHAR_WIDTH_9BIT (1)

// OR-ed IRQ flags which are allowed to be used by the user
#define MP_UART_ALLOWED_FLAGS (UART_FLAG_IDLE | UART_FLAG_RXNE)

// OR-ed IRQ flags which should not be touched by the user
#define MP_UART_RESERVED_FLAGS UART_FLAG_RXNE

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    USART_TypeDef *uartx;
    pyb_uart_t uart_id : 8;
    bool is_static : 1;
    bool is_enabled : 1;
    bool attached_to_repl;              // whether the UART is attached to REPL
    byte char_width;                    // 0 for 7,8 bit chars, 1 for 9 bit chars
    uint16_t char_mask;                 // 0x7f for 7 bit, 0xff for 8 bit, 0x1ff for 9 bit
    uint16_t timeout;                   // timeout waiting for first char
    uint16_t timeout_char;              // timeout waiting between chars
    uint16_t read_buf_len;              // len in chars; buf can hold len-1 chars
    volatile uint16_t read_buf_head;    // indexes first empty slot
    uint16_t read_buf_tail;             // indexes first full slot (not full if equals head)
    byte *read_buf;                     // byte or uint16_t, depending on char size
    uint16_t mp_irq_trigger;            // user IRQ trigger mask
    uint16_t mp_irq_flags;              // user IRQ active IRQ flags
    mp_irq_obj_t *mp_irq_obj;           // user IRQ object
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
void uart_irq_handler(mp_uint_t uart_id);

void uart_attach_to_repl(machine_uart_obj_t *self, bool attached);
uint32_t uart_get_source_freq(machine_uart_obj_t *self);
uint32_t uart_get_baudrate(machine_uart_obj_t *self);
void uart_set_baudrate(machine_uart_obj_t *self, uint32_t baudrate);

mp_uint_t uart_rx_any(machine_uart_obj_t *uart_obj);
bool uart_rx_wait(machine_uart_obj_t *self, uint32_t timeout);
int uart_rx_char(machine_uart_obj_t *uart_obj);
bool uart_tx_wait(machine_uart_obj_t *self, uint32_t timeout);
size_t uart_tx_data(machine_uart_obj_t *self, const void *src_in, size_t num_chars, int *errcode);
void uart_tx_strn(machine_uart_obj_t *uart_obj, const char *str, uint len);

static inline bool uart_tx_avail(machine_uart_obj_t *self) {
    #if defined(STM32F4) || defined(STM32L1)
    return self->uartx->SR & USART_SR_TXE;
    #elif defined(STM32G0) || defined(STM32H7) || defined(STM32WL)
    return self->uartx->ISR & USART_ISR_TXE_TXFNF;
    #else
    return self->uartx->ISR & USART_ISR_TXE;
    #endif
}

#endif // MICROPY_INCLUDED_STM32_UART_H
