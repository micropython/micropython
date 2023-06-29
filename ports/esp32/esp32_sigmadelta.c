/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Mike Causer <mcauser@gmail.com>
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

#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"
#include "driver/sigmadelta.h"

// Forward declaration
extern const mp_obj_type_t esp32_sigmadelta_type;

typedef struct _esp32_sigmadelta_obj_t {
    mp_obj_base_t base;
    uint8_t channel_id;
    gpio_num_t pin;
    int8_t duty;
    uint8_t prescale;
} esp32_sigmadelta_obj_t;

STATIC mp_obj_t esp32_sigmadelta_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_channel,   MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_pin,       MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_duty,                        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_prescale,                    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 80} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_uint_t channel_id = args[0].u_int;
    gpio_num_t pin_id = machine_pin_get_id(args[1].u_obj);
    mp_int_t duty = args[2].u_int;
    mp_uint_t prescale = args[3].u_int;

    if (duty < -128 || duty > 127) {
        mp_raise_ValueError(MP_ERROR_TEXT("duty must be between -128 and 127"));
    }

    if (prescale > 255) {
        mp_raise_ValueError(MP_ERROR_TEXT("prescale must be between 0 and 255"));
    }

    esp32_sigmadelta_obj_t *self = m_new_obj_with_finaliser(esp32_sigmadelta_obj_t);
    self->base.type = &esp32_sigmadelta_type;
    self->channel_id = channel_id;
    self->pin = pin_id;
    self->duty = duty;
    self->prescale = prescale;

    sigmadelta_config_t config;
    config.channel = (sigmadelta_channel_t)self->channel_id;
    config.sigmadelta_gpio = self->pin;
    config.sigmadelta_duty = self->duty;
    config.sigmadelta_prescale = self->prescale;

    check_esp_err(sigmadelta_config(&config));

    return MP_OBJ_FROM_PTR(self);
}

STATIC void esp32_sigmadelta_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_sigmadelta_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin != -1) {
        mp_printf(print, "SigmaDelta(channel=%u, pin=%u, duty=%d, prescale=%u)",
            self->channel_id, self->pin, self->duty, self->prescale);
    } else {
        mp_printf(print, "SigmaDelta()");
    }
}

STATIC mp_obj_t esp32_sigmadelta_deinit(mp_obj_t self_in) {
    esp32_sigmadelta_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pin != -1) { // Check if channel has already been deinitialised.
        gpio_matrix_out(self->pin, SIG_GPIO_OUT_IDX, 0, 0);
        self->pin = -1; // -1 to indicate SigmaDelta is unused
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_sigmadelta_deinit_obj, esp32_sigmadelta_deinit);

STATIC mp_obj_t esp32_sigmadelta_duty(size_t n_args, const mp_obj_t *args) {
    esp32_sigmadelta_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t duty;
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->duty);
    }

    // set
    duty = mp_obj_get_int(args[1]);
    if (duty < -128 || duty > 127) {
        mp_raise_ValueError(MP_ERROR_TEXT("duty must be between -128 and 127"));
    }
    check_esp_err(sigmadelta_set_duty(self->channel_id, duty));
    self->duty = duty;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_sigmadelta_duty_obj, 1, 2, esp32_sigmadelta_duty);

STATIC mp_obj_t esp32_sigmadelta_prescale(size_t n_args, const mp_obj_t *args) {
    esp32_sigmadelta_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t prescale;
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->prescale);
    }

    // set
    prescale = mp_obj_get_int(args[1]);
    if (prescale > 255) {
        mp_raise_ValueError(MP_ERROR_TEXT("prescale must be between 0 and 255"));
    }
    check_esp_err(sigmadelta_set_prescale(self->channel_id, prescale));
    self->prescale = prescale;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_sigmadelta_prescale_obj, 1, 2, esp32_sigmadelta_prescale);

STATIC const mp_rom_map_elem_t esp32_sigmadelta_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&esp32_sigmadelta_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_sigmadelta_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&esp32_sigmadelta_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_prescale), MP_ROM_PTR(&esp32_sigmadelta_prescale_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_sigmadelta_locals_dict, esp32_sigmadelta_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_sigmadelta_type,
    MP_QSTR_SIGMADELTA,
    MP_TYPE_FLAG_NONE
    make_new, esp32_sigmadelta_make_new,
    print, esp32_sigmadelta_print,
    locals_dict, &esp32_sigmadelta_locals_dict
    );
