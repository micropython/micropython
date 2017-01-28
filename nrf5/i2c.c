/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "i2c.h"
#include "hal_twi.h"

#if MICROPY_PY_MACHINE_HW_I2C

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    TWI_HandleTypeDef *i2c;
} machine_i2c_obj_t;

TWI_HandleTypeDef I2CHandle0 = {.instance = NULL, .init.id = 0};
TWI_HandleTypeDef I2CHandle1 = {.instance = NULL, .init.id = 1};

STATIC const machine_i2c_obj_t machine_i2c_obj[] = {
    {{&machine_i2c_type}, &I2CHandle0},
    {{&machine_i2c_type}, &I2CHandle1},
};

void i2c_init0(void) {
    // reset the I2C handles
    memset(&I2CHandle0, 0, sizeof(TWI_HandleTypeDef));
    I2CHandle0.instance = TWI_BASE(0);
    memset(&I2CHandle1, 0, sizeof(TWI_HandleTypeDef));
    I2CHandle0.instance = TWI_BASE(1);
}

STATIC int i2c_find(mp_obj_t id) {
    // given an integer id
    int i2c_id = mp_obj_get_int(id);
    if (i2c_id >= 0 && i2c_id <= MP_ARRAY_SIZE(machine_i2c_obj)
        && machine_i2c_obj[i2c_id].i2c != NULL) {
        return i2c_id;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "I2C(%d) does not exist", i2c_id));
}

STATIC void i2c_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = o;
    mp_printf(print, "I2C(%u)", self->i2c->init.id);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

// for make_new
enum {
    ARG_NEW_id,
};

STATIC mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_NEW_id].u_obj == MP_OBJ_NEW_SMALL_INT(-1)) {
        // index -1 does not exist
        return mp_const_none;
        // TODO: raise exception
    }

    // get static peripheral object
    int i2c_id = i2c_find(args[ARG_NEW_id].u_obj);
    const machine_i2c_obj_t *self = &machine_i2c_obj[i2c_id];

    hal_twi_init(self->i2c->instance, &self->i2c->init);

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    // instance methods
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&machine_i2c_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&machine_i2c_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_ready), (mp_obj_t)&machine_i2c_is_ready_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan), (mp_obj_t)&machine_i2c_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&machine_i2c_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&machine_i2c_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_read), (mp_obj_t)&machine_i2c_mem_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_write), (mp_obj_t)&machine_i2c_mem_write_obj },
#endif
    // class constants
    /// \constant MASTER - for initialising the bus to master mode
    /// \constant SLAVE - for initialising the bus to slave mode
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),       MP_OBJ_NEW_SMALL_INT(HAL_TWI_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),        MP_OBJ_NEW_SMALL_INT(HAL_TWI_SLAVE) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t machine_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = i2c_print,
    .make_new = machine_i2c_make_new,
#if 0
    .locals_dict = (mp_obj_t)&machine_i2c_locals_dict
#endif
};

#endif // MICROPY_PY_MACHINE_HW_I2C
