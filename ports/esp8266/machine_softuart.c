/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "ets_sys.h"
#include "mem.h"
#include "softuart.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/misc.h"
#include "py/mphal.h"
#include "modmachine.h"

typedef struct {
    mp_obj_base_t base;
    // uint8_t uart_id;
    Softuart *softuart_ptr; // point to instance of driver object
    pyb_pin_obj_t *tx;
    pyb_pin_obj_t *rx;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint32_t baudrate;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
} pyb_softuart_obj_t;

STATIC const char *_parity_name[] = {"None", "ODD", "EVEN"};

/******************************************************************************/
// MicroPython bindings for softUART

STATIC void pyb_softuart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_softuart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SoftUART(tx=%u, rx=%u, baudrate=%u, bits=%u, parity=%s, stop=%u, timeout=%u, timeout_char=%u)",
        mp_obj_get_pin(self->tx), mp_obj_get_pin(self->rx),
        self->baudrate, self->bits, _parity_name[self->parity],
        self->stop, self->timeout, self->timeout_char);
}

STATIC void pyb_softuart_init_helper(pyb_softuart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_timeout, ARG_timeout_char};
    static const mp_arg_t allowed_args[] = {
        // Only supported via constructor (analogous to UART id)
        // { MP_QSTR_tx, MP_ARG_REQUIRED | MP_ARG_OBJ },
        // { MP_QSTR_rx, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 10} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set baudrate (0 = default)
    if (args[ARG_baudrate].u_int < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("negative baudrate"));
    } else if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    // set data bits
    switch (args[ARG_bits].u_int) {
        case 0:
            break; // keep default
        case 8:
            // TODO set self->softuart_ptr bits if supported
            self->bits = 8;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported or invalid data bits"));
            break;
    }

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            // TODO disable self->softuart_ptr parity if supported
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            // TODO enable self->softuart_ptr parity if supported
            if (parity & 1) {
                // TODO enable ODD parity
                // self->parity = 1;
                mp_raise_ValueError(MP_ERROR_TEXT("unsupported parity"));
            } else {
                // TODO enable EVEN parity
                // self->parity = 2;
                mp_raise_ValueError(MP_ERROR_TEXT("unsupported parity"));
            }
        }
    }

    // set stop bits
    switch (args[ARG_stop].u_int) {
        case 0:
            break; // keep default
        case 1:
            // TODO set self->softuart_ptr stop bits if supported
            self->stop = 1;
            break;
        case 2:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported stop bits"));
            // self->stop = 2;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
            break;
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

    // setup
    Softuart_Init(self->softuart_ptr, self->baudrate);
}

STATIC mp_obj_t pyb_softuart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    enum { ARG_tx, ARG_rx };

    // check for 2 required arguments (tx, rx pins)
    // keeping the convention of machine.UART, later occuring
    // kwargs will be compatible with init()
    mp_arg_check_num(n_args, n_kw, 2, MP_OBJ_FUN_ARGS_MAX, true);

    // create instance
    pyb_softuart_obj_t *self = m_new_obj(pyb_softuart_obj_t);
    self->base.type = &pyb_softuart_type;

    // assign pointer to driver structure
    self->softuart_ptr = m_new_obj(Softuart);

    // assign the pins
    // TODO make tx-only, rx-only UART possible (by passing None)
    self->tx = mp_obj_get_pin_obj(args[ARG_tx]);
    uint tx_pin = mp_obj_get_pin(self->tx);
    if (Softuart_IsGpioValid(tx_pin)) {
        Softuart_SetPinTx(self->softuart_ptr, tx_pin);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid tx pin"));
    }

    self->rx = mp_obj_get_pin_obj(args[ARG_rx]);
    uint rx_pin = mp_obj_get_pin(self->rx);
    if (Softuart_IsGpioValid(rx_pin)) {
        Softuart_SetPinRx(self->softuart_ptr, rx_pin);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid rx pin"));
    }

    // set defaults
    self->baudrate = 9600;
    self->bits = 8;
    self->parity = 0;
    self->stop = 1;
    self->timeout = 1000;
    self->timeout_char = 10;

    // init the peripheral
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    pyb_softuart_init_helper(self, n_args - 2, args + 2, &kw_args); // skip 2 arguments

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_softuart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // omit self (args[0]) and call init helper
    pyb_softuart_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_softuart_init_obj, 0, pyb_softuart_init);

STATIC mp_obj_t pyb_softuart_flush(mp_obj_t self_in) {
    pyb_softuart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    Softuart_Flush(self->softuart_ptr); // reset the rx buffer to empty
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_softuart_flush_obj, pyb_softuart_flush);

STATIC const mp_rom_map_elem_t pyb_softuart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_softuart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&pyb_softuart_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },

};
STATIC MP_DEFINE_CONST_DICT(pyb_softuart_locals_dict, pyb_softuart_locals_dict_table);

STATIC mp_uint_t MP_FASTCODE(pyb_softuart_read)(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_softuart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // wait for first char to become available
    if (!Softuart_rxWait(self->softuart_ptr, self->timeout * 1000)) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // read the data
    uint8_t *buf = buf_in;
    while (Softuart_Available(self->softuart_ptr)) {
        *buf++ = Softuart_Read(self->softuart_ptr);
        if (--size == 0 || !Softuart_rxWait(self->softuart_ptr, self->timeout_char * 1000)) {
            // return number of bytes read
            return buf - (uint8_t *)buf_in;
        }
    }
    return 0; // FIXME
}

STATIC mp_uint_t pyb_softuart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    pyb_softuart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    /* TODO implement non-blocking
    // wait to be able to write the first character
    if (!uart_tx_wait(self, timeout)) {
        *errcode = EAGAIN;
        return MP_STREAM_ERROR;
    }
    */

    // write the data
    for (size_t i = 0; i < size; ++i) {
        Softuart_Putchar(self->softuart_ptr, *buf++);
        // FIXME //uart_tx_one_char(self->uart_id, *buf++);
    }

    // return number of bytes written
    return size;
}

STATIC mp_uint_t pyb_softuart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_softuart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && Softuart_rx_any(self->softuart_ptr)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && Softuart_tx_any_room(self->softuart_ptr)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t softuart_stream_p = {
    .read = pyb_softuart_read,
    .write = pyb_softuart_write,
    .ioctl = pyb_softuart_ioctl,
    .is_text = false,
};

const mp_obj_type_t pyb_softuart_type = {
    { &mp_type_type },
    .name = MP_QSTR_SoftUART,
    .print = pyb_softuart_print,
    .make_new = pyb_softuart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &softuart_stream_p,
    .locals_dict = (mp_obj_dict_t *)&pyb_softuart_locals_dict,
};
