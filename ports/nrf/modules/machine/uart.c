/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Glenn Ruben Bakke
 * Copyright (c) 2018 Ayke van Laethem
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

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "pin.h"
#include "genhdr/pins.h"

#include "uart.h"
#include "mpconfigboard.h"
#include "nrf.h"
#include "mphalport.h"
#include "nrfx_uart.h"


#if MICROPY_PY_MACHINE_UART

typedef struct _machine_hard_uart_obj_t {
    mp_obj_base_t       base;
    const nrfx_uart_t * p_uart;      // Driver instance
} machine_hard_uart_obj_t;

static const nrfx_uart_t instance0 = NRFX_UART_INSTANCE(0);

STATIC const machine_hard_uart_obj_t machine_hard_uart_obj[] = {
    {{&machine_hard_uart_type}, .p_uart = &instance0},
};

void uart_init0(void) {
}

STATIC int uart_find(mp_obj_t id) {
    // given an integer id
    int uart_id = mp_obj_get_int(id);
    if (uart_id >= 0 && uart_id < MP_ARRAY_SIZE(machine_hard_uart_obj)) {
        return uart_id;
    }
    mp_raise_ValueError("UART doesn't exist");
}

void uart_irq_handler(mp_uint_t uart_id) {

}

bool uart_rx_any(const machine_hard_uart_obj_t *uart_obj) {
    // TODO: uart will block for now.
    return true;
}

int uart_rx_char(const machine_hard_uart_obj_t * self) {
    uint8_t ch;
    nrfx_uart_rx(self->p_uart, &ch, 1);
    return (int)ch;
}

STATIC nrfx_err_t uart_tx_char(const machine_hard_uart_obj_t * self, int c) {
    while (nrfx_uart_tx_in_progress(self->p_uart)) {
        ;
    }

    return nrfx_uart_tx(self->p_uart, (uint8_t *)&c, 1);
}


void uart_tx_strn(const machine_hard_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        uart_tx_char(uart_obj, *str);
    }
}

void uart_tx_strn_cooked(const machine_hard_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(uart_obj, '\r');
        }
        uart_tx_char(uart_obj, *str);
    }
}

/******************************************************************************/
/* MicroPython bindings                                                      */

STATIC void machine_hard_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
}



/// \method init(id, baudrate)
///
/// Initialise the UART bus with the given parameters:
///   - `id`is bus id.
///   - `baudrate` is the clock rate.
STATIC mp_obj_t machine_hard_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int uart_id = uart_find(args[ARG_id].u_obj);
    const machine_hard_uart_obj_t * self = &machine_hard_uart_obj[uart_id];

    nrfx_uart_config_t config;

    // flow control
#if MICROPY_HW_UART1_HWFC
    config.hwfc = NRF_UART_HWFC_ENABLED;
#else
    config.hwfc = NRF_UART_HWFC_DISABLED;
#endif

    config.parity = NRF_UART_PARITY_EXCLUDED;

#if (BLUETOOTH_SD == 100)
    config.interrupt_priority = 3;
#else
    config.interrupt_priority = 6;
#endif

    // These baudrates are not supported, it seems.
    if (args[ARG_baudrate].u_int < 1200 || args[ARG_baudrate].u_int > 1000000) {
        mp_raise_ValueError("UART baudrate not supported");
    }

    // Magic: calculate 'baudrate' register from the input number.
    // Every value listed in the datasheet will be converted to the
    // correct register value, except for 192600. I believe the value
    // listed in the nrf52 datasheet (0x0EBED000) is incorrectly rounded
    // and should be 0x0EBEE000, as the nrf51 datasheet lists the
    // nonrounded value 0x0EBEDFA4.
    // Some background:
    // https://devzone.nordicsemi.com/f/nordic-q-a/391/uart-baudrate-register-values/2046#2046
    config.baudrate = args[ARG_baudrate].u_int / 400 * (uint32_t)(400ULL * (uint64_t)UINT32_MAX / 16000000ULL);
    config.baudrate = (config.baudrate + 0x800) & 0xffffff000; // rounding

    config.pseltxd = MICROPY_HW_UART1_TX;
    config.pselrxd = MICROPY_HW_UART1_RX;

#if MICROPY_HW_UART1_HWFC
    config.pselrts = MICROPY_HW_UART1_RTS;
    config.pselcts = MICROPY_HW_UART1_CTS;
#endif

    // Set context to this instance of UART
    config.p_context = (void *)self;

    // Set NULL as callback function to keep it blocking
    nrfx_uart_init(self->p_uart, &config, NULL);

    nrfx_uart_rx_enable(self->p_uart);

    return MP_OBJ_FROM_PTR(self);
}

/// \method writechar(char)
/// Write a single character on the bus.  `char` is an integer to write.
/// Return value: `None`.
STATIC mp_obj_t machine_hard_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    machine_hard_uart_obj_t *self = self_in;

    // get the character to write (might be 9 bits)
    int data = mp_obj_get_int(char_in);

    nrfx_err_t err = uart_tx_char(self, data);
    if (err != NRFX_SUCCESS) {
        mp_hal_raise(err);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_hard_uart_writechar_obj, machine_hard_uart_writechar);

/// \method readchar()
/// Receive a single character on the bus.
/// Return value: The character read, as an integer.  Returns -1 on timeout.
STATIC mp_obj_t machine_hard_uart_readchar(mp_obj_t self_in) {
    machine_hard_uart_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(uart_rx_char(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_hard_uart_readchar_obj, machine_hard_uart_readchar);

// uart.sendbreak()
STATIC mp_obj_t machine_hard_uart_sendbreak(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_hard_uart_sendbreak_obj, machine_hard_uart_sendbreak);

STATIC const mp_rom_map_elem_t machine_hard_uart_locals_dict_table[] = {
    // instance methods
    /// \method read([nbytes])
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    /// \method readline()
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    /// \method readinto(buf[, nbytes])
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    /// \method writechar(buf)
    { MP_ROM_QSTR(MP_QSTR_writechar), MP_ROM_PTR(&machine_hard_uart_writechar_obj) },
    { MP_ROM_QSTR(MP_QSTR_readchar), MP_ROM_PTR(&machine_hard_uart_readchar_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_hard_uart_sendbreak_obj) },

    // class constants
/*
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) },
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) },
*/
};

STATIC MP_DEFINE_CONST_DICT(machine_hard_uart_locals_dict, machine_hard_uart_locals_dict_table);

STATIC mp_uint_t machine_hard_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    const machine_hard_uart_obj_t *self = self_in;
    byte *buf = buf_in;

    // read the data
    for (size_t i = 0; i < size; i++) {
        buf[i] = uart_rx_char(self);
    }

    return size;
}

STATIC mp_uint_t machine_hard_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_hard_uart_obj_t *self = self_in;
    const byte *buf = buf_in;

    nrfx_err_t err = NRFX_SUCCESS;
    for (int i = 0; i < size; i++) {
        err = uart_tx_char(self, (int)((uint8_t *)buf)[i]);
    }

    if (err == NRFX_SUCCESS) {
        // return number of bytes written
        return size;
    } else {
        *errcode = mp_hal_status_to_errno_table[err];
        return MP_STREAM_ERROR;
    }
}

STATIC mp_uint_t machine_hard_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_hard_uart_obj_t *self = self_in;
    (void)self;
    return MP_STREAM_ERROR;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_hard_uart_read,
    .write = machine_hard_uart_write,
    .ioctl = machine_hard_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t machine_hard_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = machine_hard_uart_print,
    .make_new = machine_hard_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t*)&machine_hard_uart_locals_dict,
};

#endif // MICROPY_PY_MACHINE_UART
