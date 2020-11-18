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

#if MICROPY_PY_MACHINE_I2C

#include "extmod/machine_i2c.h"
#include "i2c.h"
#if NRFX_TWI_ENABLED
#include "nrfx_twi.h"
#else
#include "nrfx_twim.h"
#endif

#if NRFX_TWIM_ENABLED

#define nrfx_twi_t        nrfx_twim_t
#define nrfx_twi_config_t nrfx_twim_config_t

#define nrfx_twi_init     nrfx_twim_init
#define nrfx_twi_enable   nrfx_twim_enable
#define nrfx_twi_xfer     nrfx_twim_xfer
#define nrfx_twi_disable  nrfx_twim_disable

#define nrfx_twi_xfer_desc_t nrfx_twim_xfer_desc_t

#define NRFX_TWI_XFER_DESC_RX NRFX_TWIM_XFER_DESC_RX
#define NRFX_TWI_XFER_DESC_TX NRFX_TWIM_XFER_DESC_TX

#define NRFX_TWI_INSTANCE NRFX_TWIM_INSTANCE

#define NRF_TWI_FREQ_400K NRF_TWIM_FREQ_400K

#endif

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
    mp_raise_ValueError(MP_ERROR_TEXT("I2C doesn't exist"));
}

STATIC void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = self_in;
    mp_printf(print, "I2C(%u)", self->p_twi.drv_inst_idx);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args);

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

int machine_hard_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    nrfx_twi_enable(&self->p_twi);

    nrfx_err_t err_code;
    int transfer_ret = 0;
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        nrfx_twi_xfer_desc_t desc = NRFX_TWI_XFER_DESC_RX(addr, buf, len);
        err_code = nrfx_twi_xfer(&self->p_twi, &desc, 0);
    } else {
        nrfx_twi_xfer_desc_t desc = NRFX_TWI_XFER_DESC_TX(addr, buf, len);
        err_code = nrfx_twi_xfer(&self->p_twi, &desc, (flags & MP_MACHINE_I2C_FLAG_STOP) == 0);
        transfer_ret = len;
    }

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

    return transfer_ret;
}

STATIC const mp_machine_i2c_p_t machine_hard_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_hard_i2c_transfer_single,
};

const mp_obj_type_t machine_hard_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_hard_i2c_print,
    .make_new = machine_hard_i2c_make_new,
    .protocol = &machine_hard_i2c_p,
    .locals_dict = (mp_obj_dict_t*)&mp_machine_i2c_locals_dict,
};

#endif // MICROPY_PY_MACHINE_I2C
