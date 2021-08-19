/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 SummerGift <zhangyuan@rt-thread.com>
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
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/stream.h"

#include <stdarg.h>
#include "machine_uart.h"

#ifdef MICROPYTHON_USING_MACHINE_UART

#ifndef RT_USING_SERIAL
#error "Please define the RT_USING_SERIAL on 'rtconfig.h'"
#endif

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    struct rt_serial_device *uart_device;
}machine_uart_obj_t;

STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = (machine_uart_obj_t*) self_in;
    mp_printf(print, "uart( device port : %s,baud_rate = %d, data_bits = %d, parity = %d, stop_bits = %d )",
            self->uart_device->parent.parent.name,
            self->uart_device->config.baud_rate,
            self->uart_device->config.data_bits,
            self->uart_device->config.parity,
            self->uart_device->config.stop_bits);
}

STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    char uart_dev_name[RT_NAME_MAX];
    snprintf(uart_dev_name, sizeof(uart_dev_name), "uart%d", mp_obj_get_int(args[0]));

    struct rt_serial_device *rt_serial_device = (struct rt_serial_device *) rt_device_find(uart_dev_name);
    if (rt_serial_device == RT_NULL || rt_serial_device->parent.type != RT_Device_Class_Char) {
        mp_printf(&mp_plat_print, "ERROR: UART device %s not found!\n", uart_dev_name);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%s) doesn't exist", uart_dev_name));
    }
	
    rt_err_t result;
    result = rt_device_open((rt_device_t)rt_serial_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );
    if (result != RT_EOK)
    {
        mp_printf(&mp_plat_print, "ERROR: UART device %s can't open!\n", uart_dev_name);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%s) can't open", uart_dev_name));
    }

    // create new uart object
    machine_uart_obj_t *self = m_new_obj(machine_uart_obj_t);
    self->base.type = &machine_uart_type;
    self->uart_device = rt_serial_device;
    return (mp_obj_t) self;
}

/// \method init(baudrate, bits=8, parity=None, stop=1, *, timeout=1000, timeout_char=0, flow=0, read_buf_len=64)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 7, 8 or 9.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `stop` is the number of stop bits, 1 or 2.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_char` is the timeout in milliseconds to wait between characters.
///   - `flow` is RTS | CTS where RTS == 256, CTS == 512
///   - `read_buf_len` is the character length of the read buffer (0 to disable).
///
STATIC mp_obj_t machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },        // rt-thread does not support
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };

    // parse args
    struct {
        mp_arg_val_t baudrate, bits, parity, stop, flow, timeout, timeout_char, read_buf_len;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t*)&args);

    // set the UART configuration values
    struct rt_serial_device *uart_p = self->uart_device;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    // baudrate
    config.baud_rate = args.baudrate.u_int;

    // parity
    mp_int_t bits = args.bits.u_int;
    if (args.parity.u_obj == mp_const_none) {
        config.parity = PARITY_NONE;
    } else {
        mp_int_t parity = mp_obj_get_int(args.parity.u_obj);
        config.parity = (parity & 1) ? PARITY_ODD : PARITY_EVEN;
        //bits += 1; // STs convention has bits including parity, not all mcu
    }

    // number of bits
    if (bits == 8) {
        config.data_bits = DATA_BITS_8;
    } else if (bits == 9) {
        config.data_bits = DATA_BITS_9;
    } else if (bits == 7) {
        config.data_bits = DATA_BITS_7;
    } else {
        mp_raise_ValueError("unsupported combination of bits and parity");
    }

    // stop bits
    switch (args.stop.u_int) {
        case 1: config.stop_bits = STOP_BITS_1; break;
        default: config.stop_bits = STOP_BITS_2; break;
    }

    //buffer size
    config.bufsz = args.read_buf_len.u_int;

    rt_device_control((struct rt_device *) uart_p, RT_DEVICE_CTRL_CONFIG, &config);
    return mp_const_none;
}


STATIC mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

STATIC mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

#define RETRY_TIMES 500

STATIC mp_obj_t machine_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    machine_uart_obj_t *self = self_in;
    uint16_t data = mp_obj_get_int(char_in);
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    do
    {
        len = rt_device_write((struct rt_device *)(self->uart_device), 0, &data, 1);
        timeout++;
    }
    while (len != 1 && timeout < RETRY_TIMES);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_writechar_obj, machine_uart_writechar);

#define UART_RX_EVENT (1 << 0)
static struct rt_event event;

STATIC mp_obj_t machine_uart_readchar(mp_obj_t self_in) {
    machine_uart_obj_t *self = self_in;
    rt_uint32_t e;
    rt_uint8_t ch;

    while (rt_device_read((struct rt_device *)(self->uart_device), 0, &ch, 1) != 1) {
        rt_event_recv(&event, UART_RX_EVENT, RT_EVENT_FLAG_AND |
        RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e);
    }

    return MP_OBJ_NEW_SMALL_INT(ch);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_readchar_obj, machine_uart_readchar);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    // instance methods

    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },
//    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },

    /// \method read([nbytes])
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    /// \method readline()
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    /// \method readinto(buf[, nbytes])
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    /// \method write(buf)
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },

    { MP_ROM_QSTR(MP_QSTR_writechar), MP_ROM_PTR(&machine_uart_writechar_obj) },
    { MP_ROM_QSTR(MP_QSTR_readchar), MP_ROM_PTR(&machine_uart_readchar_obj) },
//    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&pyb_uart_sendbreak_obj) },

//    class constants
//    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) },
//    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) },
};
STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = self_in;
    byte *buf = buf_in;
    //TODO dfs sync read
    //MP_RTT_NOT_IMPL_PRINT;
    return rt_device_read((struct rt_device *)(self->uart_device), -1, buf, size);
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = self_in;
    const byte *buf = buf_in;
    //TODO dfs sync write
    //MP_RTT_NOT_IMPL_PRINT;
    return rt_device_write((struct rt_device *)(self->uart_device), -1, buf, size);
}

STATIC mp_uint_t machine_uart_ioctl(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode) {
    return NULL;
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
    .locals_dict = (mp_obj_dict_t*)&machine_uart_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_UART
