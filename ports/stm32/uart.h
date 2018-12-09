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
} pyb_uart_t;

#define CHAR_WIDTH_8BIT (0)
#define CHAR_WIDTH_9BIT (1)

typedef struct _pyb_uart_obj_t {
    mp_obj_base_t base;
    UART_HandleTypeDef uart;            // this is 17 words big
    IRQn_Type irqn;
    pyb_uart_t uart_id : 8;
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
} pyb_uart_obj_t;

extern const mp_obj_type_t pyb_uart_type;

void uart_init0(void);
void uart_deinit_all(void);
bool uart_exists(int uart_id);
bool uart_init2(pyb_uart_obj_t *uart_obj);
void uart_deinit(pyb_uart_obj_t *uart_obj);
void uart_irq_handler(mp_uint_t uart_id);

void uart_attach_to_repl(pyb_uart_obj_t *self, bool attached);
uint32_t uart_get_baudrate(pyb_uart_obj_t *self);
mp_uint_t uart_rx_any(pyb_uart_obj_t *uart_obj);
bool uart_rx_wait(pyb_uart_obj_t *self, uint32_t timeout);
int uart_rx_char(pyb_uart_obj_t *uart_obj);
bool uart_tx_wait(pyb_uart_obj_t *self, uint32_t timeout);
size_t uart_tx_data(pyb_uart_obj_t *self, const void *src_in, size_t num_chars, int *errcode);
void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len);

#endif // MICROPY_INCLUDED_STM32_UART_H
