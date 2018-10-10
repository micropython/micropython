/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_i2c.h"
#include "i2c.h"
#include "nrfx_twi.h"

#if MICROPY_PY_MACHINE_I2C

STATIC const mp_obj_type_t machine_hard_i2c_type;

typedef struct _machine_hard_i2c_obj_t {
    mp_obj_base_t base;
    nrfx_twi_t    p_twi;  // Driver instance
} machine_hard_i2c_obj_t;

STATIC const machine_hard_i2c_obj_t machine_hard_i2c_obj[] = {
    {{&machine_hard_i2c_type}, .p_twi = NRFX_TWI_INSTANCE(0)},
    {{&machine_hard_i2c_type}, .p_twi = NRFX_TWI_INSTANCE(1)},
};

void i2c_init0(void) {
}

STATIC int i2c_find(mp_obj_t id) {
    // given an integer id
    int i2c_id = mp_obj_get_int(id);
    if (i2c_id >= 0 && i2c_id < MP_ARRAY_SIZE(machine_hard_i2c_obj)) {
        return i2c_id;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "I2C(%d) does not exist", i2c_id));
}

STATIC void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = self_in;
    mp_printf(print, "I2C(%u)", self->p_twi.drv_inst_idx);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scl,      MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda,      MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int i2c_id = i2c_find(args[ARG_id].u_obj);
    const machine_hard_i2c_obj_t *self = &machine_hard_i2c_obj[i2c_id];

    nrfx_twi_config_t config;
    config.scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj)->pin;
    config.sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj)->pin;

    config.frequency = NRF_TWI_FREQ_400K;

    config.hold_bus_uninit = false;

    // Set context to this object.
    nrfx_twi_init(&self->p_twi, &config, NULL, (void *)self);

    return MP_OBJ_FROM_PTR(self);
}

int machine_hard_i2c_readfrom(mp_obj_base_t *self_in, uint16_t addr, uint8_t *dest, size_t len, bool stop) {
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    nrfx_twi_enable(&self->p_twi);

    nrfx_err_t err_code = nrfx_twi_rx(&self->p_twi, addr, dest, len);

    if (err_code != NRFX_SUCCESS) {
        if (err_code == NRFX_ERROR_DRV_TWI_ERR_ANACK) {
            return -MP_ENODEV;
        }
        else if (err_code == NRFX_ERROR_DRV_TWI_ERR_DNACK) {
            return -MP_EIO;
        }
        return -MP_ETIMEDOUT;
    }

    nrfx_twi_disable(&self->p_twi);

    return 0;
}

int machine_hard_i2c_writeto(mp_obj_base_t *self_in, uint16_t addr, const uint8_t *src, size_t len, bool stop) {
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    nrfx_twi_enable(&self->p_twi);

    nrfx_err_t err_code = nrfx_twi_tx(&self->p_twi, addr, src, len, !stop);

    if (err_code != NRFX_SUCCESS) {
        if (err_code == NRFX_ERROR_DRV_TWI_ERR_ANACK) {
            return -MP_ENODEV;
        }
        else if (err_code == NRFX_ERROR_DRV_TWI_ERR_DNACK) {
            return -MP_EIO;
        }
        return -MP_ETIMEDOUT;
    }

    nrfx_twi_disable(&self->p_twi);

    return len;
}

STATIC const mp_machine_i2c_p_t machine_hard_i2c_p = {
    .readfrom = machine_hard_i2c_readfrom,
    .writeto = machine_hard_i2c_writeto,
};

STATIC const mp_obj_type_t machine_hard_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_hard_i2c_print,
    .make_new = machine_hard_i2c_make_new,
    .protocol = &machine_hard_i2c_p,
    .locals_dict = (mp_obj_dict_t*)&mp_machine_soft_i2c_locals_dict,
};

#endif // MICROPY_PY_MACHINE_I2C
