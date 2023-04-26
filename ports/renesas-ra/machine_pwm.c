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

// #include <stdio.h>
// #include <string.h>"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_pwm.h"
#include "pin.h"
#include "ra/ra_gpt.h"
#include "modmachine.h"

#if MICROPY_HW_ENABLE_HW_PWM

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

STATIC machine_pwm_obj_t machine_pwm_obj[] = {
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

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(GTIOC %d%c[#%d], runing=%u, freq=%u, duty=%u)", self->ch, self->id, self->pwm->pin, self->active, self->freq, self->duty);
}

STATIC void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    uint32_t D = 0ul;

    enum { ARG_freq, ARG_duty /*, ARG_duty_u16, ARG_duty_ns*/ };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} }
        // { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        // { MP_QSTR_duty_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_freq].u_int != -1) {
        if ((args[ARG_freq].u_int < 0) || (args[ARG_freq].u_int > 24000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("\033[91mfreq should be [0 - 24000000] Hz\033[0m"));
        } else {
            self->freq = args[ARG_freq].u_int;
        }
    }

    ra_gpt_timer_init(self->pwm->pin, self->ch, self->id, 0, (float)self->freq);

    if (args[ARG_duty].u_int != -1) {
        if ((args[ARG_duty].u_int < 0) || (args[ARG_duty].u_int > 100)) {
            mp_raise_ValueError(MP_ERROR_TEXT("\033[91mduty should be [0 - 100] %%\033[0m"));
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

// STATIC mp_obj_t    machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args)
STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
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
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("\033[91mPin(%d) has no timer Output function\033[0m"), self->pwm->pin);
            }
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("\033[91mPin(%d) exist, but is used with other periphery (ALT function)\033[0m"), pin_id->pin);
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("\033[91mPin missing or does not exist!\033[0m"));
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    ra_gpt_timer_deinit(self->pwm->pin, self->ch, self->id);
    self->active = 0;
    self->ch = 0;
    self->id = ' ';
    self->duty = 0;
    self->freq = 0;
}

STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT((uint32_t)ra_gpt_timer_get_freq(self->ch));
}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    if (freq) {
        ra_gpt_timer_set_freq(self->ch, (float)freq);
        self->freq = (uint32_t)ra_gpt_timer_get_freq(self->ch);
        if (!self->freq) {
            mp_raise_ValueError(MP_ERROR_TEXT("\033[91mcan't set freq, It should be [0 - 24000000] Hz\033[0m"));
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

STATIC mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    // give the result in %
    uint64_t Dc = ra_gpt_timer_get_duty(self->ch, self->id) * 100;
    return MP_OBJ_NEW_SMALL_INT(Dc / ra_gpt_timer_get_period(self->ch));
}

STATIC void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    // assume duty is in %
    if (duty < 0 || duty > 100) {
        mp_raise_ValueError(MP_ERROR_TEXT("\033[91mduty should be value in [0 - 100] %%\033[0m"));
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

/*
STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    // give the result in ratio (u16 / 65535)
    return MP_OBJ_NEW_SMALL_INT(?);
}

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    //assume duty is a ratio (u16 / 65535)
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    // give the result in ns
    return MP_OBJ_NEW_SMALL_INT(?);
}

STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    //assume duty is ns
}
*/

#endif // MICROPY_HW_ENABLE_HW_PWM
