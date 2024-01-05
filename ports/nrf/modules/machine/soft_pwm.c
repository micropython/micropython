/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Robert Hammelrath
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
#include "softpwm.h"

typedef enum {
    DUTY_NOT_SET = 0,
    DUTY,
    DUTY_U16,
    DUTY_NS
} pwm_duty_t;

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t pwm_pin;
    bool defer_start;
    uint8_t duty_mode;
    uint32_t duty;
    uint32_t freq;
} machine_pwm_obj_t;

#define SOFT_PWM_BASE_FREQ      (1000000)
#define DUTY_FULL_SCALE         (1024)

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = self_in;
    static char *duty_suffix[] = { "", "", "_u16", "_ns" };
    mp_printf(print, "<PWM: Pin=%u freq=%dHz duty%s=%d>",
        self->pwm_pin, self->freq,
        duty_suffix[self->duty_mode], self->duty);
}

// MicroPython bindings for machine API

static void machine_soft_pwm_start(machine_pwm_obj_t *self);

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->defer_start = true;
    if (args[ARG_freq].u_int != -1) {
        mp_machine_pwm_freq_set(self, args[ARG_freq].u_int);
    }
    if (args[ARG_duty].u_int != -1) {
        mp_machine_pwm_duty_set(self, args[ARG_duty].u_int);
    }
    if (args[ARG_duty_u16].u_int != -1) {
        mp_machine_pwm_duty_set_u16(self, args[ARG_duty_u16].u_int);
    }
    if (args[ARG_duty_ns].u_int != -1) {
        mp_machine_pwm_duty_set_ns(self, args[ARG_duty_ns].u_int);
    }
    self->defer_start = false;
    // (Re-)start the PWM.
    machine_soft_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    enum { ARG_pin, ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns, ARG_id };

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // check if the PWM pin is valid.
    int pwm_pin = mp_hal_get_pin_obj(args[0])->pin;
    if (pwm_pin > 31) {
        mp_raise_ValueError(MP_ERROR_TEXT("Pin number >31"));
    }

    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->defer_start = false;
    self->pwm_pin = pwm_pin;
    self->duty_mode = DUTY_NOT_SET;
    self->duty = 0;
    self->freq = 0;

    // parse the remaining arguments and start the PWM
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    pwm_release(self->pwm_pin);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->freq);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {

    if (freq > (SOFT_PWM_BASE_FREQ / 256)) {
        mp_raise_ValueError(MP_ERROR_TEXT("frequency out of range"));
    }
    self->freq = freq;
    machine_soft_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    if (self->duty_mode) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else if (self->duty_mode == DUTY_U16) {
        return MP_OBJ_NEW_SMALL_INT(self->duty * 100 / 65536);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    self->duty = duty;
    self->duty_mode = DUTY;
    machine_soft_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    if (self->duty_mode == DUTY_U16) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else if (self->duty_mode == DUTY) {
        return MP_OBJ_NEW_SMALL_INT(self->duty * 65536 / 100);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty) {
    self->duty = duty;
    self->duty_mode = DUTY_U16;
    machine_soft_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    if (self->duty_mode == DUTY_NS) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty) {
    self->duty = duty;
    self->duty_mode = DUTY_NS;
    machine_soft_pwm_start(self);
}

/* Interface for the implementation */

static void machine_soft_pwm_start(machine_pwm_obj_t *self) {

    // check if ready to go
    if (self->defer_start == true || self->freq == 0 || self->duty_mode == DUTY_NOT_SET) {
        return; // Not ready yet.
    }

    int ret = pwm_set_period_us(SOFT_PWM_BASE_FREQ / self->freq);

    if (ret >= 0) {
        int duty_width;
        if (self->duty_mode == DUTY) {
            duty_width = self->duty * DUTY_FULL_SCALE / 100;
        } else if (self->duty_mode == DUTY_U16) {
            duty_width = self->duty * DUTY_FULL_SCALE / 65536;
        } else if (self->duty_mode == DUTY_NS) {
            duty_width = (uint64_t)self->duty * self->freq * DUTY_FULL_SCALE / 1000000000ULL;
        }
        pwm_set_duty_cycle(self->pwm_pin, duty_width);
    }
}
