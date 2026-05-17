/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Artem Makarov
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "modesp32.h"

#if SOC_GP_LDO_SUPPORTED

#include "esp_ldo_regulator.h"

// LDO (Low-Dropout Regulator) Driver
//
// This module provides access to the internal LDO voltage regulators,
// which can supply adjustable voltages to internal peripherals or external components.

typedef struct _esp32_ldo_obj_t {
    mp_obj_base_t base;
    esp_ldo_channel_handle_t ldo_channel_handle;
    int chan_id; // Cached for display
    int voltage_mv; // Cached for display
} esp32_ldo_obj_t;

static mp_obj_t esp32_ldo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_channel_id, MP_ARG_REQUIRED | MP_ARG_INT, },
        {MP_QSTR_voltage_mv, MP_ARG_REQUIRED | MP_ARG_INT, },
        {MP_QSTR_adjustable, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    esp_ldo_channel_config_t channel_config = {
        .chan_id = args[0].u_int,
        .voltage_mv = args[1].u_int,
        .flags.adjustable = args[2].u_bool,
    };

    esp32_ldo_obj_t *self = mp_obj_malloc(esp32_ldo_obj_t, &esp32_ldo_type);
    self->ldo_channel_handle = NULL;
    self->chan_id = args[0].u_int;
    self->voltage_mv = args[1].u_int;

    check_esp_err(esp_ldo_acquire_channel(&channel_config, &self->ldo_channel_handle));
    return MP_OBJ_FROM_PTR(self);
}

static void esp32_ldo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    const esp32_ldo_obj_t *self = self_in;
    if (self->ldo_channel_handle != NULL) {
        mp_printf(print, "LDO(channel=%d, voltage=%dmV)", self->chan_id, self->voltage_mv);
    } else {
        mp_printf(print, "LDO(released)");
    }
}

static mp_obj_t esp32_ldo_adjust_voltage(mp_obj_t self_in, mp_obj_t voltage_mv_in) {
    esp32_ldo_obj_t *self = self_in;
    mp_int_t voltage_mv = mp_obj_get_int(voltage_mv_in);

    check_esp_err(esp_ldo_channel_adjust_voltage(self->ldo_channel_handle, voltage_mv));
    self->voltage_mv = voltage_mv;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(esp32_ldo_adjust_voltage_obj, esp32_ldo_adjust_voltage);

static mp_obj_t esp32_ldo_release(mp_obj_t self_in) {
    esp32_ldo_obj_t *self = self_in;
    if (self->ldo_channel_handle != NULL) {
        check_esp_err(esp_ldo_release_channel(self->ldo_channel_handle));
        self->ldo_channel_handle = NULL;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(esp32_ldo_release_obj, esp32_ldo_release);

static mp_obj_t esp32_ldo_del(mp_obj_t self_in) {
    esp32_ldo_obj_t *self = self_in;
    if (self->ldo_channel_handle != NULL) {
        esp_ldo_release_channel(self->ldo_channel_handle);
        self->ldo_channel_handle = NULL;
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ldo_del_obj, esp32_ldo_del);

static mp_obj_t esp32_ldo_enter(mp_obj_t self_in) {
    return self_in;
}

static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ldo_enter_obj, esp32_ldo_enter);

static mp_obj_t esp32_ldo_exit(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return esp32_ldo_release(args[0]);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_ldo_exit_obj, 4, 4, esp32_ldo_exit);

static const mp_rom_map_elem_t esp32_ldo_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_adjust_voltage), MP_ROM_PTR(&esp32_ldo_adjust_voltage_obj)},
    {MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&esp32_ldo_release_obj)},
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&esp32_ldo_del_obj)},
    {MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&esp32_ldo_enter_obj)},
    {MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&esp32_ldo_exit_obj)},
};

static MP_DEFINE_CONST_DICT(esp32_ldo_locals_dict, esp32_ldo_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_ldo_type,
    MP_QSTR_LDO,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_ldo_make_new,
    print, esp32_ldo_print,
    locals_dict, &esp32_ldo_locals_dict
    );

#endif // SOC_GP_LDO_SUPPORTED
