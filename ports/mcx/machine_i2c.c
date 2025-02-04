/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2024 NXP
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

#include "extmod/modmachine.h"

#include "drv_i2c.h"

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    uint32_t frequency;
    drv_i2c_t drv;
    uint32_t timeout;
} machine_i2c_obj_t;

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "I2C(%u, freq=%u)", self->id, self->frequency);
}

static void mp_machine_i2c_init_helper(machine_i2c_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_scl, ARG_sda, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 500}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    drv_i2c_config_t i2c_cfg;
    self->frequency = args[ARG_freq].u_int;
    i2c_cfg.frequency = self->frequency;

    if (args[ARG_scl].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for SCL*/
    } else {
        mp_hal_pin_obj_t pin = args[ARG_scl].u_obj;

        /* TODO: Non LP-Flexcomm pinmap for future A series */
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P1);
        if (af == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported SCL AF"));
        }

        mp_hal_pin_af_od(pin, af->af_id);
        mp_hal_pin_pull_up(pin);
    }

    if (args[ARG_sda].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for SCL*/
    } else {
        mp_hal_pin_obj_t pin = args[ARG_sda].u_obj;

        /* TODO: Non LP-Flexcomm pinmap for future A series */
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P0);
        if (af == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported SDA AF"));
        }

        mp_hal_pin_af_od(pin, af->af_id);
        mp_hal_pin_pull_up(pin);
    }

    self->timeout = args[ARG_timeout].u_int;

    /* TODO: Check return values */
    drv_i2c_init(&self->drv, self->id, &i2c_cfg);
}

mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);


    if (!mp_obj_is_int(args[0])) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported I2C id."));
        return mp_const_none;
    }

    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, &machine_i2c_type);

    self->id = mp_obj_get_int(args[0]);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_i2c_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = MP_OBJ_FROM_PTR(self_in);

    int ret;

    bool stop = (flags & MP_MACHINE_I2C_FLAG_STOP) ? true : false;
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        ret = drv_i2c_read(&self->drv, addr, buf, len, self->timeout, stop);
        if (ret < 0) {
            return ret;
        }
    } else {
        ret = drv_i2c_write(&self->drv, addr, buf, len, self->timeout, stop);
        if (ret < 0) {
            return ret;
        }
    }

    return len;
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    protocol, &machine_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );
