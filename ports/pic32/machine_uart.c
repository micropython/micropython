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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"

// PIC32MX UART Register Definitions
#define UART1_BASE  0xBF806000
#define UART2_BASE  0xBF806200
#define UART3_BASE  0xBF806400
#define UART4_BASE  0xBF806600
#define UART5_BASE  0xBF806800
#define UART6_BASE  0xBF806A00

// UART Register Offsets
#define UxMODE      0x00
#define UxMODECLR   0x04
#define UxMODESET   0x08
#define UxMODEINV   0x0C
#define UxSTA       0x10
#define UxSTACLR    0x14
#define UxSTASET    0x18
#define UxSTAINV    0x1C
#define UxTXREG     0x20
#define UxRXREG     0x30
#define UxBRG       0x40

// UxMODE bits
#define UARTEN      (1 << 15)  // UART Enable
#define PDSEL_8N    (0 << 1)   // 8-bit data, no parity
#define PDSEL_8E    (1 << 1)   // 8-bit data, even parity
#define PDSEL_8O    (2 << 1)   // 8-bit data, odd parity
#define PDSEL_9N    (3 << 1)   // 9-bit data, no parity
#define STSEL       (1 << 0)   // Stop Selection (1=2 stop bits)

// UxSTA bits
#define URXEN       (1 << 12)  // Receiver Enable
#define UTXEN       (1 << 10)  // Transmitter Enable
#define UTXBF       (1 << 9)   // Transmit Buffer Full
#define TRMT        (1 << 8)   // Transmit Shift Register Empty
#define URXDA       (1 << 0)   // Receive Data Available

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t uart_id;           // 0-5 for UART1-6
    uint32_t baudrate;
    uint8_t bits;              // 8 or 9
    uint8_t parity;            // 0=none, 1=even, 2=odd
    uint8_t stop;              // 1 or 2
    uint16_t timeout;          // milliseconds
    uint16_t timeout_char;     // milliseconds
    uint32_t base_addr;
} machine_uart_obj_t;

STATIC uint32_t get_uart_base(uint8_t uart_id) {
    switch (uart_id) {
        case 0: return UART1_BASE;
        case 1: return UART2_BASE;
        case 2: return UART3_BASE;
        case 3: return UART4_BASE;
        case 4: return UART5_BASE;
        case 5: return UART6_BASE;
        default: return UART1_BASE;
    }
}

STATIC void uart_init(machine_uart_obj_t *self) {
    uint32_t base = self->base_addr;
    volatile uint32_t *mode = (volatile uint32_t *)(base + UxMODE);
    volatile uint32_t *sta = (volatile uint32_t *)(base + UxSTA);
    volatile uint32_t *brg = (volatile uint32_t *)(base + UxBRG);

    // Disable UART
    *mode &= ~UARTEN;

    // Configure baud rate
    // BRG = (PBCLK / (16 * baudrate)) - 1
    // Assuming PBCLK = 40 MHz
    uint32_t pbclk = 40000000;
    *brg = (pbclk / (16 * self->baudrate)) - 1;

    // Configure mode
    uint32_t mode_val = 0;

    // Data bits and parity
    if (self->bits == 9) {
        mode_val |= PDSEL_9N;
    } else {
        if (self->parity == 1) {
            mode_val |= PDSEL_8E;
        } else if (self->parity == 2) {
            mode_val |= PDSEL_8O;
        } else {
            mode_val |= PDSEL_8N;
        }
    }

    // Stop bits
    if (self->stop == 2) {
        mode_val |= STSEL;
    }

    *mode = mode_val;

    // Enable UART, TX and RX
    *mode |= UARTEN;
    *sta |= (UTXEN | URXEN);
}

// UART(id, baudrate=9600, bits=8, parity=None, stop=1, timeout=1000)
STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 6, true);

    // Get UART ID
    mp_int_t uart_id = mp_obj_get_int(args[0]);
    if (uart_id < 1 || uart_id > 6) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART%d doesn't exist"), uart_id);
    }

    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, type);
    self->uart_id = uart_id - 1;  // 0-5 internal
    self->base_addr = get_uart_base(self->uart_id);
    self->baudrate = 9600;
    self->bits = 8;
    self->parity = 0;
    self->stop = 1;
    self->timeout = 1000;
    self->timeout_char = 0;

    // Parse arguments
    if (n_args >= 2) {
        self->baudrate = mp_obj_get_int(args[1]);
    }
    if (n_args >= 3) {
        self->bits = mp_obj_get_int(args[2]);
    }
    if (n_args >= 4 && args[3] != mp_const_none) {
        self->parity = mp_obj_get_int(args[3]);
    }
    if (n_args >= 5) {
        self->stop = mp_obj_get_int(args[4]);
    }
    if (n_args >= 6) {
        self->timeout = mp_obj_get_int(args[5]);
    }

    uart_init(self);

    return MP_OBJ_FROM_PTR(self);
}

// uart.read([nbytes])
STATIC mp_obj_t machine_uart_read(size_t n_args, const mp_obj_t *args) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t base = self->base_addr;
    volatile uint32_t *sta = (volatile uint32_t *)(base + UxSTA);
    volatile uint32_t *rxreg = (volatile uint32_t *)(base + UxRXREG);

    size_t len = 64;  // Default
    if (n_args > 1) {
        len = mp_obj_get_int(args[1]);
    }

    vstr_t vstr;
    vstr_init_len(&vstr, len);

    size_t bytes_read = 0;
    while (bytes_read < len) {
        // Check if data available
        if (*sta & URXDA) {
            vstr.buf[bytes_read++] = *rxreg & 0xFF;
        } else {
            break;  // No more data
        }
    }

    vstr.len = bytes_read;
    return mp_obj_new_bytes_from_vstr(&vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_uart_read_obj, 1, 2, machine_uart_read);

// uart.write(buf)
STATIC mp_obj_t machine_uart_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t base = self->base_addr;
    volatile uint32_t *sta = (volatile uint32_t *)(base + UxSTA);
    volatile uint32_t *txreg = (volatile uint32_t *)(base + UxTXREG);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    size_t bytes_written = 0;
    uint8_t *buf = bufinfo.buf;

    for (size_t i = 0; i < bufinfo.len; i++) {
        // Wait for TX buffer to be available
        while (*sta & UTXBF) {
            // Busy wait
        }
        *txreg = buf[i];
        bytes_written++;
    }

    return MP_OBJ_NEW_SMALL_INT(bytes_written);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_write_obj, machine_uart_write);

// uart.any()
STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t base = self->base_addr;
    volatile uint32_t *sta = (volatile uint32_t *)(base + UxSTA);

    return MP_OBJ_NEW_SMALL_INT((*sta & URXDA) ? 1 : 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_uart_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_uart_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_NONE,
    make_new, machine_uart_make_new,
    locals_dict, &machine_uart_locals_dict
);
