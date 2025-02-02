/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 * Copyright (c) 2020 Yonatan Schachter
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

#include "py/mperrno.h"
#include "zephyr_device.h"

// The UART class doesn't have any constants for this port.
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
} machine_uart_obj_t;

static const char *_parity_name[] = {"None", "Odd", "Even", "Mark", "Space"};
static const char *_stop_bits_name[] = {"0.5", "1", "1.5", "2"};
static const char *_data_bits_name[] = {"5", "6", "7", "8", "9"};
static const char *_flow_control_name[] = {"None", "RTS/CTS", "DTR/DSR"};

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct uart_config config;
    uart_config_get(self->dev, &config);
    mp_printf(print, "UART(\"%s\", baudrate=%u, data_bits=%s, parity_bits=%s, stop_bits=%s, flow_control=%s, timeout=%u, timeout_char=%u)",
        self->dev->name, config.baudrate, _data_bits_name[config.data_bits],
        _parity_name[config.parity], _stop_bits_name[config.stop_bits], _flow_control_name[config.flow_ctrl],
        self->timeout, self->timeout_char);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_timeout, ARG_timeout_char };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->timeout = args[ARG_timeout].u_int;
    self->timeout_char = args[ARG_timeout_char].u_int;
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
    self->dev = zephyr_device_find(args[0]);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    (void)self;
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    (void)self;
    mp_raise_NotImplementedError(NULL); // TODO
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    (void)self;
    mp_raise_NotImplementedError(NULL); // TODO
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *buffer = (uint8_t *)buf_in;
    uint8_t data;
    mp_uint_t bytes_read = 0;
    size_t elapsed_ms = 0;
    size_t time_to_wait = self->timeout;

    while ((elapsed_ms < time_to_wait) && (bytes_read < size)) {
        if (!uart_poll_in(self->dev, &data)) {
            buffer[bytes_read++] = data;
            elapsed_ms = 0;
            time_to_wait = self->timeout_char;
        } else {
            k_msleep(1);
            elapsed_ms++;
        }
    }
    return bytes_read;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *buffer = (uint8_t *)buf_in;

    for (mp_uint_t i = 0; i < size; i++) {
        uart_poll_out(self->dev, buffer[i]);
    }

    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_uint_t ret;

    if (request == MP_STREAM_POLL) {
        ret = 0;
        // read is always blocking

        if (arg & MP_STREAM_POLL_WR) {
            ret |= MP_STREAM_POLL_WR;
        }
        return ret;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
