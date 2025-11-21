/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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

// UART implementation for bare-arm port (STM32F4)

#include "py/runtime.h"
#include "py/mphal.h"

// STM32F4 UART register definitions
#define USART1_BASE 0x40011000
#define USART2_BASE 0x40004400
#define USART3_BASE 0x40004800

#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840)
#define RCC_APB2ENR (*(volatile uint32_t *)0x40023844)

typedef struct {
    volatile uint32_t SR;    // Status register
    volatile uint32_t DR;    // Data register
    volatile uint32_t BRR;   // Baud rate register
    volatile uint32_t CR1;   // Control register 1
    volatile uint32_t CR2;   // Control register 2
    volatile uint32_t CR3;   // Control register 3
} USART_TypeDef;

#define USART1 ((USART_TypeDef *)USART1_BASE)
#define USART2 ((USART_TypeDef *)USART2_BASE)
#define USART3 ((USART_TypeDef *)USART3_BASE)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    USART_TypeDef *uart;
    uint8_t uart_id;
    uint32_t baudrate;
} machine_uart_obj_t;

static USART_TypeDef *get_uart(uint8_t uart_id) {
    switch (uart_id) {
        case 1: return USART1;
        case 2: return USART2;
        case 3: return USART3;
        default: return USART1;
    }
}

static void uart_clock_enable(uint8_t uart_id) {
    switch (uart_id) {
        case 1: RCC_APB2ENR |= (1 << 4); break;  // USART1
        case 2: RCC_APB1ENR |= (1 << 17); break; // USART2
        case 3: RCC_APB1ENR |= (1 << 18); break; // USART3
    }
}

static mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, type);

    self->uart_id = mp_obj_get_int(args[0]);
    self->baudrate = (n_args >= 2) ? mp_obj_get_int(args[1]) : 9600;
    self->uart = get_uart(self->uart_id);

    // Enable UART clock
    uart_clock_enable(self->uart_id);

    // Configure UART
    // Assuming 84MHz APB1 clock for USART2/3, 168MHz APB2 for USART1
    uint32_t apb_clock = (self->uart_id == 1) ? 168000000 : 84000000;
    uint32_t brr = apb_clock / self->baudrate;

    self->uart->BRR = brr;
    self->uart->CR1 = (1 << 13) | (1 << 3) | (1 << 2);  // UE | TE | RE

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((self->uart->SR & (1 << 5)) ? 1 : 0);  // RXNE
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

static mp_obj_t machine_uart_read(mp_obj_t self_in, mp_obj_t len_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t len = mp_obj_get_int(len_in);

    vstr_t vstr;
    vstr_init_len(&vstr, len);

    for (int i = 0; i < len; i++) {
        // Wait for RXNE
        while (!(self->uart->SR & (1 << 5)));
        ((uint8_t *)vstr.buf)[i] = self->uart->DR & 0xFF;
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_read_obj, machine_uart_read);

static mp_obj_t machine_uart_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    for (size_t i = 0; i < bufinfo.len; i++) {
        // Wait for TXE
        while (!(self->uart->SR & (1 << 7)));
        self->uart->DR = ((uint8_t *)bufinfo.buf)[i];
    }

    return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_write_obj, machine_uart_write);

static const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_uart_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_uart_write_obj) },
};
static MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_NONE,
    make_new, machine_uart_make_new,
    locals_dict, &machine_uart_locals_dict
);
