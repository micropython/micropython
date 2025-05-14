/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
 * Copyright (c) 2019, NXP
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

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "zephyr_device.h"

#if MICROPY_PY_MACHINE_I2C

typedef struct _machine_hard_i2c_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    bool restart;
} machine_hard_i2c_obj_t;

static void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = self_in;
    mp_printf(print, "%s", self->dev->name);
}

mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const struct device *dev = zephyr_device_find(args[ARG_id].u_obj);

    if ((args[ARG_scl].u_obj != MP_OBJ_NULL) || (args[ARG_sda].u_obj != MP_OBJ_NULL)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("explicit choice of scl/sda is not implemented"));
    }

    if ((args[ARG_freq].u_obj != MP_OBJ_NULL)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("explicit choice of freq is not implemented"));
    }

    if ((args[ARG_timeout].u_obj != MP_OBJ_NULL)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("explicit choice of timeout is not implemented"));
    }

    machine_hard_i2c_obj_t *self = mp_obj_malloc(machine_hard_i2c_obj_t, &machine_i2c_type);
    self->dev = dev;
    self->restart = false;

    return MP_OBJ_FROM_PTR(self);
}

static int machine_hard_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;
    struct i2c_msg msg;
    int ret;

    msg.buf = (uint8_t *)buf;
    msg.len = len;
    msg.flags = 0;

    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        msg.flags |= I2C_MSG_READ;
    } else {
        msg.flags |= I2C_MSG_WRITE;
    }

    if (self->restart) {
        msg.flags |= I2C_MSG_RESTART;
    }

    if (flags & MP_MACHINE_I2C_FLAG_STOP) {
        msg.flags |= I2C_MSG_STOP;
        self->restart = false;
    } else {
        self->restart = true;
    }

    ret = i2c_transfer(self->dev, &msg, 1, addr);
    return (ret < 0) ? -MP_EIO : len;
}

static const mp_machine_i2c_p_t machine_hard_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_hard_i2c_transfer_single,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_hard_i2c_make_new,
    print, machine_hard_i2c_print,
    protocol, &machine_hard_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );

#endif // MICROPY_PY_MACHINE_I2C
