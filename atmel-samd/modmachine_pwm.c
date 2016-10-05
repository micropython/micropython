/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "py/runtime.h"
#include "modmachine_pin.h"
#include "modmachine_pwm.h"

#include "asf/sam0/drivers/tc/tc.h"
#include "asf/sam0/drivers/tcc/tcc.h"

typedef struct _pwm_obj_t {
    mp_obj_base_t base;
    const pin_obj_t *pin;
    bool using_primary_timer;
    struct tc_module tc_instance;
    struct tcc_module tcc_instance;
} pwm_obj_t;

/******************************************************************************/
// MicroPython bindings for PWM

STATIC void pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%s", self->pin->name);
    mp_printf(print, ", freq=%u, duty=%u", 0, 0);
    mp_printf(print, ")");
}
/// \classmethod \constructor(pin)
/// Create a PWM object associated with the given pin. This allows you to write PWM
/// signals out on the given pin. Frequency is currently fixed at ~735Hz like Arduino.
STATIC mp_obj_t pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_obj_t pin_obj = args[0];
    const pin_obj_t *pin = pin_find(pin_obj);

    // create PWM object from the given pin
    pwm_obj_t *self = m_new_obj(pwm_obj_t);
    self->base.type = &pwm_type;
    self->pin = pin;
    self->using_primary_timer = true;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    enum { ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    if (pin->primary_timer.tc == 0 && pin->primary_timer.tcc == 0 &&
       pin->secondary_timer.tc == 0 && pin->secondary_timer.tcc == 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "PWM not supported on pin %q", self->pin->name));
    }

    if (parsed_args[ARG_duty].u_int < 0 || parsed_args[ARG_duty].u_int > 255) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "PWM duty must be between 0 and 255 (8 bit resolution), not %d",
            parsed_args[ARG_duty].u_int));
    }
    uint8_t duty = parsed_args[ARG_duty].u_int;

    // TODO(tannewt): Support output on multiple timer channels at once.
    const pin_timer_t* t = &pin->primary_timer;
    if (t->tcc != 0) {
        struct tcc_config config_tcc;
        tcc_get_config_defaults(&config_tcc, t->tcc);

        config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
        config_tcc.counter.period = 0xFF;
        config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
        config_tcc.compare.match[t->channel] = duty;

        config_tcc.pins.enable_wave_out_pin[t->wave_output] = true;
        config_tcc.pins.wave_out_pin[t->wave_output] = t->pin;
        config_tcc.pins.wave_out_pin_mux[t->wave_output] = t->mux;

        tcc_init(&self->tcc_instance, t->tcc, &config_tcc);

        tcc_enable(&self->tcc_instance);
    } else {
        struct tc_config config_tc;
        tc_get_config_defaults(&config_tc);

        config_tc.counter_size    = TC_COUNTER_SIZE_8BIT;
        config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
        config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;
        config_tc.counter_8_bit.period = 0xff;
        config_tc.counter_8_bit.compare_capture_channel[t->channel] = duty;

        config_tc.pwm_channel[t->wave_output].enabled = true;
        config_tc.pwm_channel[t->wave_output].pin_out = t->pin;
        config_tc.pwm_channel[t->wave_output].pin_mux = t->mux;

        tc_init(&self->tc_instance, t->tc, &config_tc);

        tc_enable(&self->tc_instance);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pwm_del(mp_obj_t self_in) {
    pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const pin_timer_t* t = &self->pin->primary_timer;
    if (!self->using_primary_timer) {
      t = &self->pin->secondary_timer;
    }
    if (t->tcc != 0) {
      tcc_disable(&self->tcc_instance);
    } else {
      tc_disable(&self->tc_instance);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pwm_del_obj, pwm_del);

// 8 bit resolution
STATIC mp_obj_t pwm_duty(size_t n_args, const mp_obj_t *args) {
    pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    const pin_timer_t* t = &self->pin->primary_timer;
    if (!self->using_primary_timer) {
        t = &self->pin->secondary_timer;
    }

    if (n_args == 1) {
        uint8_t duty;
        if (t->tcc != 0) {
            duty = tcc_get_capture_value(&self->tcc_instance, t->channel);
        } else {
            duty = tc_get_capture_value(&self->tc_instance, t->channel);
        }
        return MP_OBJ_NEW_SMALL_INT(duty);
    } else {
        mp_int_t duty = mp_obj_get_int(args[1]);
        if (duty < 0 || duty > 255) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "PWM duty must be between 0 and 255 (8 bit resolution), not %d",
                duty));
        }
        if (t->tcc != 0) {
            tcc_set_compare_value(&self->tcc_instance, t->channel, duty);
        } else {
            tc_set_compare_value(&self->tc_instance, t->channel, duty);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pwm_duty_obj, 1, 2, pwm_duty);

STATIC const mp_rom_map_elem_t pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&pwm_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&pwm_duty_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pwm_locals_dict, pwm_locals_dict_table);

const mp_obj_type_t pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = pwm_print,
    .make_new = pwm_make_new,
    .locals_dict = (mp_obj_dict_t*)&pwm_locals_dict,
};
