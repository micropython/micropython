/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Glenn Ruben Bakke
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
#include "hal_uart.h"

#define CHAR_WIDTH_8BIT (0)
#define CHAR_WIDTH_9BIT (1)

struct _pyb_uart_obj_t {
    mp_obj_base_t base;
    UART_HandleTypeDef uart;
    IRQn_Type irqn;
    pyb_uart_t uart_id : 8;
    bool is_enabled : 1;
    byte char_width;                    // 0 for 7,8 bit chars, 1 for 9 bit chars
    uint16_t char_mask;                 // 0x7f for 7 bit, 0xff for 8 bit, 0x1ff for 9 bit
    uint16_t timeout;                   // timeout waiting for first char
    uint16_t timeout_char;              // timeout waiting between chars
    uint16_t read_buf_len;              // len in chars; buf can hold len-1 chars
    volatile uint16_t read_buf_head;    // indexes first empty slot
    uint16_t read_buf_tail;             // indexes first full slot (not full if equals head)
    byte *read_buf;                     // byte or uint16_t, depending on char size
};

STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in);

void uart_init0(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_uart_obj_all)); i++) {
        MP_STATE_PORT(pyb_uart_obj_all)[i] = NULL;
    }
}

// unregister all interrupt sources
void uart_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_uart_obj_all)); i++) {
        pyb_uart_obj_t *uart_obj = MP_STATE_PORT(pyb_uart_obj_all)[i];
        if (uart_obj != NULL) {
            pyb_uart_deinit(uart_obj);
        }
    }
}

/// \method deinit()
/// Turn off the UART bus.
STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_deinit_obj, pyb_uart_deinit);

//// assumes Init parameters have been set up correctly
STATIC bool uart_init2(pyb_uart_obj_t * uart_obj) {
    uart_obj->is_enabled = true;

    return true;
}

void uart_irq_handler(mp_uint_t uart_id) {

}

bool uart_rx_any(pyb_uart_obj_t *uart_obj) {
	// TODO: uart will block for now.
	return true;
}

// Waits at most timeout milliseconds for at least 1 char to become ready for
// reading (from buf or for direct reading).
// Returns true if something available, false if not.
STATIC bool uart_rx_wait(pyb_uart_obj_t *self, uint32_t timeout) {
	return false;
}

int uart_rx_char(pyb_uart_obj_t *self) {
    return (int)nrf_uart_char_read();
}

STATIC void uart_tx_char(pyb_uart_obj_t * self, int c) {
    nrf_uart_char_write((char)c);
}


void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        uart_tx_char(uart_obj, *str);
    }
}

void uart_tx_strn_cooked(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(uart_obj, '\r');
        }
        uart_tx_char(uart_obj, *str);
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC void pyb_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
}

/// \method init(baudrate, bits=8, parity=None, stop=1, *, timeout=1000, timeout_char=0, read_buf_len=64)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 7, 8 or 9.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `stop` is the number of stop bits, 1 or 2.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_char` is the timeout in milliseconds to wait between characters.
///   - `read_buf_len` is the character length of the read buffer (0 to disable).
STATIC mp_obj_t pyb_uart_init_helper(pyb_uart_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_HWCONTROL_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };


    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set the UART configuration values
    memset(&self->uart, 0, sizeof(self->uart));
    UART_InitTypeDef *init = &self->uart.init;

    // baudrate
    init->baud_rate = args[0].u_int;

    // flow control
    init->flow_control = args[4].u_int;

    // init UART (if it fails, it's because the port doesn't exist)
    if (!uart_init2(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", self->uart_id));
    }

    // set timeouts
    self->timeout = args[5].u_int;
    self->timeout_char = args[6].u_int;

    // setup the read buffer
    m_del(byte, self->read_buf, self->read_buf_len << self->char_width);

    self->read_buf_head = 0;
    self->read_buf_tail = 0;

    if (args[7].u_int <= 0) {
        // no read buffer
        self->read_buf_len = 0;
        self->read_buf = NULL;
    } else {
        // read buffer using interrupts
        self->read_buf_len = args[7].u_int;
        self->read_buf = m_new(byte, args[7].u_int << self->char_width);
    }

    hal_uart_init_t uart_init = {
#if MICROPY_HW_UART1_HWFC
        .flow_control = true,
#else
        .flow_control = false,
#endif
        .use_parity   = false,
        .baud_rate    = HAL_UART_BAUD_115K2,
#if (BLUETOOTH_SD == 100)
        .irq_priority = 3
#else
        .irq_priority = 6
#endif
    };
    uart_init.rx_pin = &MICROPY_HW_UART1_RX;
    uart_init.tx_pin = &MICROPY_HW_UART1_TX;

#if MICROPY_HW_UART1_HWFC
    uart_init.rts_pin = &MICROPY_HW_UART1_RTS;
    uart_init.cts_pin = &MICROPY_HW_UART1_CTS;
#endif

    nrf_uart_init(&uart_init);

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct a UART object.
STATIC mp_obj_t pyb_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    int uart_id = 0;

    if (MP_OBJ_IS_STR(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {

        } else if (strcmp(port, "COM1") == 0) {
            uart_id = PYB_UART_1;
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%s) does not exist", port));
        }
    } else {
        uart_id = mp_obj_get_int(args[0]);
        if (uart_id < 1 || uart_id > MP_ARRAY_SIZE(MP_STATE_PORT(pyb_uart_obj_all))) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", uart_id));
        }
    }

    pyb_uart_obj_t *self;
    if (MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1] == NULL) {
        // create new UART object
        self = m_new0(pyb_uart_obj_t, 1);
        self->base.type = &pyb_uart_type;
        self->uart_id = uart_id;
        MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1] = self;
    } else {
        // reference existing UART object
        self = MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1];
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return self;
}


/// \method any()
/// Return `True` if any characters waiting, else `False`.
STATIC mp_obj_t pyb_uart_any(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    if (uart_rx_any(self)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_any_obj, pyb_uart_any);

/// \method writechar(char)
/// Write a single character on the bus.  `char` is an integer to write.
/// Return value: `None`.
STATIC mp_obj_t pyb_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    pyb_uart_obj_t *self = self_in;

    // get the character to write (might be 9 bits)
    uint16_t data = mp_obj_get_int(char_in);

    for (int i = 0; i < 2; i++) {
    	uart_tx_char(self, (int)(&data)[i]);
    }

    self->uart.instance->TASKS_STOPTX  = 0;

    HAL_StatusTypeDef status = self->uart.instance->EVENTS_ERROR;

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_uart_writechar_obj, pyb_uart_writechar);

/// \method readchar()
/// Receive a single character on the bus.
/// Return value: The character read, as an integer.  Returns -1 on timeout.
STATIC mp_obj_t pyb_uart_readchar(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    
    if (uart_rx_wait(self, self->timeout)) {
        return MP_OBJ_NEW_SMALL_INT(uart_rx_char(self));
    } else {
        // return -1 on timeout
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_readchar_obj, pyb_uart_readchar);

// uart.sendbreak()
STATIC mp_obj_t pyb_uart_sendbreak(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_sendbreak_obj, pyb_uart_sendbreak);

STATIC const mp_map_elem_t pyb_uart_locals_dict_table[] = {
    // instance methods

    //{ MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_uart_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_uart_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&pyb_uart_any_obj },

    /// \method read([nbytes])
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    /// \method readline()
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    /// \method readinto(buf[, nbytes])
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_stream_readinto_obj },
    /// \method writechar(buf)
    { MP_OBJ_NEW_QSTR(MP_QSTR_writechar), (mp_obj_t)&pyb_uart_writechar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readchar), (mp_obj_t)&pyb_uart_readchar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendbreak), (mp_obj_t)&pyb_uart_sendbreak_obj },

    // class constants
/*
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTS), MP_OBJ_NEW_SMALL_INT(UART_HWCONTROL_RTS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CTS), MP_OBJ_NEW_SMALL_INT(UART_HWCONTROL_CTS) },
*/
};

STATIC MP_DEFINE_CONST_DICT(pyb_uart_locals_dict, pyb_uart_locals_dict_table);

STATIC mp_uint_t pyb_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // convert byte size to char size
    size >>= self->char_width;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // read the data
    byte * orig_buf = buf;
    for (;;) {
        int data = uart_rx_char(self);

		*buf++ = data;

        if (--size == 0) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

STATIC mp_uint_t pyb_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    const byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    for (int i = 0; i < size; i++) {
    	uart_tx_char(self, (int)((uint8_t *)buf)[i]);
    }

    HAL_StatusTypeDef status = self->uart.instance->EVENTS_ERROR;

    if (status == HAL_OK) {
        // return number of bytes written
        return size;
    } else {
        *errcode = mp_hal_status_to_errno_table[status];
        return MP_STREAM_ERROR;
    }
}

STATIC mp_uint_t pyb_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    (void)self;
    return MP_STREAM_ERROR;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = pyb_uart_read,
    .write = pyb_uart_write,
    .ioctl = pyb_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t pyb_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = pyb_uart_print,
    .make_new = pyb_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_t)&pyb_uart_locals_dict,
};

