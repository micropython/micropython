/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 * Copyright (c) 2023 Vekatech Ltd.
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
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include "py/mphal.h"
#include "py/mperrno.h"
#include "pin.h"
#include "ra/ra_gpt.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    R_GPT0_Type *pwm_inst;
    uint8_t active;
    uint8_t ch;
    uint8_t id;
    uint8_t duty;
    uint32_t freq;
    mp_hal_pin_obj_t pwm;
} machine_pwm_obj_t;

static machine_pwm_obj_t machine_pwm_obj[] = {
    #if defined(MICROPY_HW_PWM_0A)
    {{&machine_pwm_type}, R_GPT0, 0, 0, 'A', 0, 0ul, MICROPY_HW_PWM_0A},
    #endif
    #if defined(MICROPY_HW_PWM_0B)
    {{&machine_pwm_type}, R_GPT0, 0, 0, 'B', 0, 0ul, MICROPY_HW_PWM_0B},
    #endif
    #if defined(MICROPY_HW_PWM_1A)
    {{&machine_pwm_type}, R_GPT1, 0, 1, 'A', 0, 0ul, MICROPY_HW_PWM_1A},
    #endif
    #if defined(MICROPY_HW_PWM_1B)
    {{&machine_pwm_type}, R_GPT1, 0, 1, 'B', 0, 0ul, MICROPY_HW_PWM_1B},
    #endif
    #if defined(MICROPY_HW_PWM_2A)
    {{&machine_pwm_type}, R_GPT2, 0, 2, 'A', 0, 0ul, MICROPY_HW_PWM_2A},
    #endif
    #if defined(MICROPY_HW_PWM_2B)
    {{&machine_pwm_type}, R_GPT2, 0, 2, 'B', 0, 0ul, MICROPY_HW_PWM_2B},
    #endif
    #if defined(MICROPY_HW_PWM_3A)
    {{&machine_pwm_type}, R_GPT3, 0, 3, 'A', 0, 0ul, MICROPY_HW_PWM_3A},
    #endif
    #if defined(MICROPY_HW_PWM_3B)
    {{&machine_pwm_type}, R_GPT3, 0, 3, 'B', 0, 0ul, MICROPY_HW_PWM_3B},
    #endif
    #if defined(MICROPY_HW_PWM_4A)
    {{&machine_pwm_type}, R_GPT4, 0, 4, 'A', 0, 0ul, MICROPY_HW_PWM_4A},
    #endif
    #if defined(MICROPY_HW_PWM_4B)
    {{&machine_pwm_type}, R_GPT4, 0, 4, 'B', 0, 0ul, MICROPY_HW_PWM_4B},
    #endif
    #if defined(MICROPY_HW_PWM_5A)
    {{&machine_pwm_type}, R_GPT5, 0, 5, 'A', 0, 0ul, MICROPY_HW_PWM_5A},
    #endif
    #if defined(MICROPY_HW_PWM_5B)
    {{&machine_pwm_type}, R_GPT5, 0, 5, 'B', 0, 0ul, MICROPY_HW_PWM_5B},
    #endif
    #if defined(MICROPY_HW_PWM_6A)
    {{&machine_pwm_type}, R_GPT6, 0, 6, 'A', 0, 0ul, MICROPY_HW_PWM_6A},
    #endif
    #if defined(MICROPY_HW_PWM_6B)
    {{&machine_pwm_type}, R_GPT6, 0, 6, 'B', 0, 0ul, MICROPY_HW_PWM_6B},
    #endif
    #if defined(MICROPY_HW_PWM_7A)
    {{&machine_pwm_type}, R_GPT7, 0, 7, 'A', 0, 0ul, MICROPY_HW_PWM_7A},
    #endif
    #if defined(MICROPY_HW_PWM_7B)
    {{&machine_pwm_type}, R_GPT7, 0, 7, 'B', 0, 0ul, MICROPY_HW_PWM_7B},
    #endif
    #if defined(MICROPY_HW_PWM_8A)
    {{&machine_pwm_type}, R_GPT8, 0, 8, 'A', 0, 0ul, MICROPY_HW_PWM_8A},
    #endif
    #if defined(MICROPY_HW_PWM_8B)
    {{&machine_pwm_type}, R_GPT8, 0, 8, 'B', 0, 0ul, MICROPY_HW_PWM_8B},
    #endif
    #if defined(MICROPY_HW_PWM_9A)
    {{&machine_pwm_type}, R_GPT9, 0, 9, 'A', 0, 0ul, MICROPY_HW_PWM_9A},
    #endif
    #if defined(MICROPY_HW_PWM_9B)
    {{&machine_pwm_type}, R_GPT9, 0, 9, 'B', 0, 0ul, MICROPY_HW_PWM_9B},
    #endif
    #if defined(MICROPY_HW_PWM_10A)
    {{&machine_pwm_type}, R_GPT10, 0, 10, 'A', 0, 0ul, MICROPY_HW_PWM_10A},
    #endif
    #if defined(MICROPY_HW_PWM_10B)
    {{&machine_pwm_type}, R_GPT10, 0, 10, 'B', 0, 0ul, MICROPY_HW_PWM_10B},
    #endif
    #if defined(MICROPY_HW_PWM_11A)
    {{&machine_pwm_type}, R_GPT11, 0, 11, 'A', 0, 0ul, MICROPY_HW_PWM_11A},
    #endif
    #if defined(MICROPY_HW_PWM_11B)
    {{&machine_pwm_type}, R_GPT11, 0, 11, 'B', 0, 0ul, MICROPY_HW_PWM_11B},
    #endif
    #if defined(MICROPY_HW_PWM_12A)
    {{&machine_pwm_type}, R_GPT12, 0, 12, 'A', 0, 0ul, MICROPY_HW_PWM_12A},
    #endif
    #if defined(MICROPY_HW_PWM_12B)
    {{&machine_pwm_type}, R_GPT12, 0, 12, 'B', 0, 0ul, MICROPY_HW_PWM_12B},
    #endif
    #if defined(MICROPY_HW_PWM_13A)
    {{&machine_pwm_type}, R_GPT13, 0, 13, 'A', 0, 0ul, MICROPY_HW_PWM_13A},
    #endif
    #if defined(MICROPY_HW_PWM_13B)
    {{&machine_pwm_type}, R_GPT13, 0, 13, 'B', 0, 0ul, MICROPY_HW_PWM_13B}
    #endif
};

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(GTIOC %d%c[#%d], active=%u, freq=%u, duty=%u)", self->ch, self->id, self->pwm->pin, self->active, self->freq, self->duty);
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    uint32_t D = 0ul;

    enum { ARG_freq, ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_freq].u_int != -1) {
        if ((args[ARG_freq].u_int < 0) || (args[ARG_freq].u_int > 24000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq should be 0-24000000"));
        } else {
            self->freq = args[ARG_freq].u_int;
        }
    }

    ra_gpt_timer_init(self->pwm->pin, self->ch, self->id, 0, (float)self->freq);

    if (args[ARG_duty].u_int != -1) {
        if ((args[ARG_duty].u_int < 0) || (args[ARG_duty].u_int > 100)) {
            mp_raise_ValueError(MP_ERROR_TEXT("duty should be 0-100"));
        } else {
            self->duty = args[ARG_duty].u_int;
        }
    }

    D = self->duty * ra_gpt_timer_get_period(self->ch);
    ra_gpt_timer_set_duty(self->ch, self->id, (uint32_t)(D / 100));

    if (self->duty && self->freq) {
        ra_gpt_timer_start(self->ch);
        self->active = 1;
    }
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_hal_pin_obj_t pin_id = MP_OBJ_NULL;
    machine_pwm_obj_t *self = MP_OBJ_NULL;

    enum { ARG_pin, ARG_freq, ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} }
    };

    mp_arg_check_num(n_args, n_kw, 1, 3, true);
    mp_arg_val_t init_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, init_args);

    // Get GPIO and optional device to connect to PWM.
    pin_id = mp_hal_get_pin_obj(init_args[ARG_pin].u_obj);

    if (pin_id) {
        for (int i = 0; i < MP_ARRAY_SIZE(machine_pwm_obj); i++) {
            if (pin_id->pin == machine_pwm_obj[i].pwm->pin) {
                self = &machine_pwm_obj[i];
                break;
            }
        }

        if (self) {
            if (ra_gpt_timer_is_pwm_pin(self->pwm->pin)) {
                // start the PWM running for this channel
                mp_map_t kw_args;
                mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
                mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);
            } else {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%d) has no timer output"), self->pwm->pin);
            }
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%d) is used with other peripheral"), pin_id->pin);
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Pin doesn't exist"));
    }

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    ra_gpt_timer_deinit(self->pwm->pin, self->ch, self->id);
    self->active = 0;
    self->ch = 0;
    self->id = ' ';
    self->duty = 0;
    self->freq = 0;
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT((uint32_t)ra_gpt_timer_get_freq(self->ch));
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    if (freq) {
        ra_gpt_timer_set_freq(self->ch, (float)freq);
        self->freq = (uint32_t)ra_gpt_timer_get_freq(self->ch);
        if (!self->freq) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq should be 0-24000000"));
        } else
        if (!self->active) {
            ra_gpt_timer_start(self->ch);
            self->active = 1;
        }
    } else {
        ra_gpt_timer_stop(self->ch);
        ra_gpt_timer_set_freq(self->ch, (float)freq);
        self->freq = 0;
        self->active = 0;
    }
}

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    // give the result in %
    uint64_t Dc = ra_gpt_timer_get_duty(self->ch, self->id) * 100;
    return MP_OBJ_NEW_SMALL_INT(Dc / ra_gpt_timer_get_period(self->ch));
}

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    // assume duty is in %
    if (duty < 0 || duty > 100) {
        mp_raise_ValueError(MP_ERROR_TEXT("duty should be 0-100"));
    } else {
        if (duty) {
            uint64_t D = (uint8_t)(duty) * ra_gpt_timer_get_period(self->ch);
            ra_gpt_timer_set_duty(self->ch, self->id, (uint32_t)(D / 100));
            self->duty = (uint8_t)duty;

            if (!self->active && self->freq) {
                ra_gpt_timer_start(self->ch);
                self->active = 1;
            }
        } else {
            if (self->active) {
                ra_gpt_timer_stop(self->ch);
                ra_gpt_timer_set_duty(self->ch, self->id, 0);
                self->duty = 0;
                self->active = 0;
            }
        }
    }
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    // give the result in ratio (u16 / 65535)
    uint64_t Dc = ra_gpt_timer_get_duty(self->ch, self->id) * 65535;
    return MP_OBJ_NEW_SMALL_INT(Dc / ra_gpt_timer_get_period(self->ch));
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    // assume duty is a ratio (u16 / 65535)
    if (duty_u16 < 0 || duty_u16 > 65535) {
        mp_raise_ValueError(MP_ERROR_TEXT("duty should be 0-65535"));
    } else {
        if (duty_u16) {
            uint64_t D = duty_u16 * ra_gpt_timer_get_period(self->ch);
            ra_gpt_timer_set_duty(self->ch, self->id, (uint32_t)(D / 65535));
            self->duty = (uint8_t)((duty_u16 * 100) / 65535);

            if (!self->active && self->freq) {
                ra_gpt_timer_start(self->ch);
                self->active = 1;
            }
        } else {
            if (self->active) {
                ra_gpt_timer_stop(self->ch);
                ra_gpt_timer_set_duty(self->ch, self->id, 0);
                self->duty = 0;
                self->active = 0;
            }
        }
    }
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    // give the result in ns
    float ns = ra_gpt_timer_tick_time(self->ch);
    ns *= (float)ra_gpt_timer_get_duty(self->ch, self->id);
    return MP_OBJ_NEW_SMALL_INT(ns);
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    // assume duty is ns
    uint32_t ns_min = (uint32_t)ra_gpt_timer_tick_time(self->ch);
    uint32_t ns_max = ns_min * ra_gpt_timer_get_period(self->ch);
    if (duty_ns) {
        if (duty_ns < ns_min || duty_ns > ns_max) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty should be in period range %d-%d"), ns_min, ns_max);
        } else {
            uint32_t D = (uint32_t)duty_ns / ns_min;
            ra_gpt_timer_set_duty(self->ch, self->id, D);
            D *= ns_min * 100;
            self->duty = D / ns_max;

            if (!self->active && self->freq) {
                ra_gpt_timer_start(self->ch);
                self->active = 1;
            }
        }
    } else {
        if (self->active) {
            ra_gpt_timer_stop(self->ch);
            ra_gpt_timer_set_duty(self->ch, self->id, 0);
            self->duty = 0;
            self->active = 0;
        }
    }
}
