/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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
#include "extmod/modmachine.h"

#include "ra_i2c.h"

#if MICROPY_PY_MACHINE_I2C

#define DEFAULT_I2C_FREQ (400000)
#define DEFAULT_I2C_TIMEOUT (1000)

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    R_IIC0_Type *i2c_inst;
    uint8_t i2c_id;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint32_t freq;
} machine_i2c_obj_t;

static machine_i2c_obj_t machine_i2c_obj[] = {
    #if defined(MICROPY_HW_I2C0_SCL)
    {{&machine_i2c_type}, R_IIC0, 0, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA, 0},
    #endif
    #if defined(MICROPY_HW_I2C1_SCL)
    {{&machine_i2c_type}, R_IIC1, 1, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA, 0},
    #endif
    #if defined(MICROPY_HW_I2C2_SCL)
    {{&machine_i2c_type}, R_IIC2, 2, MICROPY_HW_I2C2_SCL, MICROPY_HW_I2C2_SDA, 0},
    #endif
};

static int i2c_read(machine_i2c_obj_t *self, uint16_t addr, uint8_t *dest, size_t len, bool stop);
static int i2c_write(machine_i2c_obj_t *self, uint16_t addr, const uint8_t *src, size_t len, bool stop);

static int i2c_read(machine_i2c_obj_t *self, uint16_t addr, uint8_t *dest, size_t len, bool stop) {
    bool flag;
    xaction_t action;
    xaction_unit_t unit;
    ra_i2c_xunit_init(&unit, (uint8_t *)dest, (uint32_t)len, true, (void *)NULL);
    ra_i2c_xaction_init(&action, (xaction_unit_t *)&unit, 1, (uint32_t)addr, stop);
    flag = ra_i2c_action_execute(self->i2c_inst, &action, false, DEFAULT_I2C_TIMEOUT);
    return flag? len:-1;
}

static int i2c_write(machine_i2c_obj_t *self, uint16_t addr, const uint8_t *src, size_t len, bool stop) {
    bool flag;
    xaction_t action;
    xaction_unit_t unit;
    ra_i2c_xunit_init(&unit, (uint8_t *)src, (uint32_t)len, false, (void *)NULL);
    ra_i2c_xaction_init(&action, (xaction_unit_t *)&unit, 1, (uint32_t)addr, stop);
    flag = ra_i2c_action_execute(self->i2c_inst, &action, false, DEFAULT_I2C_TIMEOUT);
    return flag? len:-1;
}

// MicroPython bindings for machine API

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u, scl=%q, sda=%q)",
        self->i2c_id, self->freq, self->scl->name, self->sda->name);
}

static void  machine_i2c_init(mp_obj_base_t *obj, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("init is not supported."));
    return;
}

static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    bool found = false;
    int i2c_id = mp_obj_get_int(args[ARG_id].u_obj);
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)&machine_i2c_obj[0];
    for (int i = 0; i < MP_ARRAY_SIZE(machine_i2c_obj); i++) {
        if (i2c_id == self->i2c_id) {
            found = true;
            break;
        }
        ++self;
    }
    if (found != true) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    // here we would check the scl/sda pins and configure them, but it's not implemented
    if (args[ARG_scl].u_obj != MP_OBJ_NULL || args[ARG_sda].u_obj != MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("explicit choice of scl/sda is not implemented"));
    }

    if (n_args > 1 || n_kw > 0 || self->freq == 0) {
        self->freq = args[ARG_freq].u_int;
        ra_i2c_init(self->i2c_inst, self->scl->pin, self->sda->pin, self->freq);
    }
    return MP_OBJ_FROM_PTR(self);
}

static int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret;
    bool stop;
    stop = (flags & MP_MACHINE_I2C_FLAG_STOP)? true : false;
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        ret = i2c_read(self, addr, buf, len, stop);
    } else {
        ret = i2c_write(self, addr, buf, len, stop);
    }
    return ret;
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .init = machine_i2c_init,
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    locals_dict, &mp_machine_i2c_locals_dict,
    print, machine_i2c_print,
    protocol, &machine_i2c_p
    );

#endif // MICROPY_PY_MACHINE_I2C
