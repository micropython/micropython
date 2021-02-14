/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "modmachine.h"

#include "hardware/uart.h"

#define DEFAULT_UART_BAUDRATE (115200)
#define DEFAULT_UART_BITS (8)
#define DEFAULT_UART_STOP (1)
#define DEFAULT_UART0_TX (0)
#define DEFAULT_UART0_RX (1)
#define DEFAULT_UART1_TX (4)
#define DEFAULT_UART1_RX (5)

#define IS_VALID_PERIPH(uart, pin)  (((((pin) + 4) & 8) >> 3) == (uart))
#define IS_VALID_TX(uart, pin)      (((pin) & 3) == 0 && IS_VALID_PERIPH(uart, pin))
#define IS_VALID_RX(uart, pin)      (((pin) & 3) == 1 && IS_VALID_PERIPH(uart, pin))

#define UART_INVERT_TX (1)
#define UART_INVERT_RX (2)
#define UART_INVERT_MASK (UART_INVERT_TX | UART_INVERT_RX)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uart_inst_t *const uart;
    uint8_t uart_id;
    uint32_t baudrate;
    uint8_t bits;
    uart_parity_t parity;
    uint8_t stop;
    uint8_t tx;
    uint8_t rx;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint8_t invert;
} machine_uart_obj_t;

STATIC machine_uart_obj_t machine_uart_obj[] = {
    {{&machine_uart_type}, uart0, 0, 0, DEFAULT_UART_BITS, UART_PARITY_NONE, DEFAULT_UART_STOP, DEFAULT_UART0_TX, DEFAULT_UART0_RX, 0, 0, 0},
    {{&machine_uart_type}, uart1, 1, 0, DEFAULT_UART_BITS, UART_PARITY_NONE, DEFAULT_UART_STOP, DEFAULT_UART1_TX, DEFAULT_UART1_RX, 0, 0, 0},
};

STATIC const char *_parity_name[] = {"None", "0", "1"};
STATIC const char *_invert_name[] = {"None", "TX", "RX", "RX, TX"};

/******************************************************************************/
// MicroPython bindings for UART

STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, tx=%d, rx=%d, timeout=%u, timeout_char=%u, invert=%s)",
        self->uart_id, self->baudrate, self->bits, _parity_name[self->parity],
        self->stop, self->tx, self->rx, self->timeout, self->timeout_char, _invert_name[self->invert]);
}

STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_timeout, ARG_timeout_char, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },

    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get UART bus.
    int uart_id = mp_obj_get_int(args[ARG_id].u_obj);
    if (uart_id < 0 || uart_id >= MP_ARRAY_SIZE(machine_uart_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
    }

    // Get static peripheral object.
    machine_uart_obj_t *self = (machine_uart_obj_t *)&machine_uart_obj[uart_id];

    // Set baudrate if configured.
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    // Set bits if configured.
    if (args[ARG_bits].u_int > 0) {
        self->bits = args[ARG_bits].u_int;
    }

    // Set parity if configured.
    if (args[ARG_parity].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            self->parity = UART_PARITY_NONE;
        } else if (mp_obj_get_int(args[ARG_parity].u_obj) & 1) {
            self->parity = UART_PARITY_ODD;
        } else {
            self->parity = UART_PARITY_EVEN;
        }
    }

    // Set stop bits if configured.
    if (args[ARG_stop].u_int > 0) {
        self->stop = args[ARG_stop].u_int;
    }

    // Set TX/RX pins if configured.
    if (args[ARG_tx].u_obj != mp_const_none) {
        int tx = mp_hal_get_pin_obj(args[ARG_tx].u_obj);
        if (!IS_VALID_TX(self->uart_id, tx)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad TX pin"));
        }
        self->tx = tx;
    }
    if (args[ARG_rx].u_obj != mp_const_none) {
        int rx = mp_hal_get_pin_obj(args[ARG_rx].u_obj);
        if (!IS_VALID_RX(self->uart_id, rx)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad RX pin"));
        }
        self->rx = rx;
    }
    // set timeout
    self->timeout = args[ARG_timeout].u_int;

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    self->timeout_char = args[ARG_timeout_char].u_int;
    uint32_t min_timeout_char = 13000 / self->baudrate + 1;
    if (self->timeout_char < min_timeout_char) {
        self->timeout_char = min_timeout_char;
    }

    // set line inversion
    if (args[ARG_invert].u_int & ~UART_INVERT_MASK) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid inversion mask"));
    }
    self->invert = args[ARG_invert].u_int;

    // Initialise the UART peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 1 || n_kw > 0 || self->baudrate == 0) {
        if (self->baudrate == 0) {
            self->baudrate = DEFAULT_UART_BAUDRATE;
        }
        uart_init(self->uart, self->baudrate);
        uart_set_format(self->uart, self->bits, self->stop, self->parity);
        uart_set_fifo_enabled(self->uart, true);
        gpio_set_function(self->tx, GPIO_FUNC_UART);
        gpio_set_function(self->rx, GPIO_FUNC_UART);
        if (self->invert & UART_INVERT_RX) {
            gpio_set_inover(self->rx, GPIO_OVERRIDE_INVERT);
        }
        if (self->invert & UART_INVERT_TX) {
            gpio_set_outover(self->tx, GPIO_OVERRIDE_INVERT);
        }
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(uart_is_readable(self->uart));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

STATIC mp_obj_t machine_uart_sendbreak(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uart_set_break(self->uart, true);
    mp_hal_delay_us(13000000 / self->baudrate + 1);
    uart_set_break(self->uart, false);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_sendbreak_obj, machine_uart_sendbreak);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },

    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_uart_sendbreak_obj) },

    { MP_ROM_QSTR(MP_QSTR_INV_TX), MP_ROM_INT(UART_INVERT_TX) },
    { MP_ROM_QSTR(MP_QSTR_INV_RX), MP_ROM_INT(UART_INVERT_RX) },

};
STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint64_t t = time_us_64() + (uint64_t)self->timeout * 1000;
    uint64_t timeout_char_us = (uint64_t)self->timeout_char * 1000;
    uint8_t *dest = buf_in;

    for (size_t i = 0; i < size; i++) {
        // Wait for the first/next character
        while (!uart_is_readable(self->uart)) {
            if (time_us_64() > t) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        *dest++ = uart_get_hw(self->uart)->dr;
        t = time_us_64() + timeout_char_us;
    }
    return size;
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint64_t t = time_us_64() + (uint64_t)self->timeout * 1000;
    uint64_t timeout_char_us = (uint64_t)self->timeout_char * 1000;
    const uint8_t *src = buf_in;

    for (size_t i = 0; i < size; i++) {
        // wait for the first/next character
        while (!uart_is_writable(self->uart)) {
            if (time_us_64() > t) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        uart_get_hw(self->uart)->dr = *src++;
        t = time_us_64() + timeout_char_us;
    }
    return size;
}

STATIC mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && uart_is_readable(self->uart)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && uart_is_writable(self->uart)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = machine_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t machine_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = machine_uart_print,
    .make_new = machine_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t *)&machine_uart_locals_dict,
};
