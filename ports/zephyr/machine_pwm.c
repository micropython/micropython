/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Ayush Singh <ayush@beagleboard.org>
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
#include "py/runtime.h"
#include "stdint.h"
#include "zephyr/drivers/pwm.h"
#include "zephyr_device.h"

#define VALUE_NOT_SET (-1)

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    const struct device *pwm;
    uint32_t channel;
    int32_t freq;
    int32_t duty_u16;
    int32_t duty_ns;
    pwm_flags_t flags;
} machine_pwm_obj_t;

static void configure_pwm(machine_pwm_obj_t *self) {
    const uint32_t period = NSEC_PER_SEC / self->freq;

    if ((self->duty_u16 == VALUE_NOT_SET && self->duty_ns == VALUE_NOT_SET) ||
        self->freq == VALUE_NOT_SET) {
        mp_raise_ValueError(MP_ERROR_TEXT("Frequency and duty values must be set"));
    }

    if (self->duty_ns == VALUE_NOT_SET) {
        self->duty_ns = (self->duty_u16 * period) / UINT16_MAX;
    }

    if (self->duty_ns < 0) {
        self->duty_ns = 0;
    } else if (self->duty_ns > period) {
        self->duty_ns = period;
    }

    pwm_set(self->pwm, self->channel, period, self->duty_ns, self->flags);
}

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in,
    mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<PWM Controller %p channel=%d ", self->pwm, self->channel);

    if (self->duty_ns != VALUE_NOT_SET) {
        mp_printf(print, " duty_ns=%d", self->duty_ns);
    } else {
        mp_printf(print, " duty_u16=%d", self->duty_u16);
    }

    mp_printf(print, " freq=%d, flags=%u>", self->freq, self->flags);
}

// This called from pwm.init() method
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args,
    const mp_obj_t *pos_args,
    mp_map_t *kw_args) {

    enum {
        ARG_freq,
        ARG_duty_u16,
        ARG_duty_ns,
        ARG_invert,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_freq, MP_ARG_INT, {.u_int = VALUE_NOT_SET}},
        {MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = VALUE_NOT_SET}},
        {MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = VALUE_NOT_SET}},
        {MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
        allowed_args, args);

    // Maybe change PWM timer
    if (args[ARG_freq].u_int > 0) {
        self->freq = args[ARG_freq].u_int;
    }

    // Set duty_u16 cycle?
    int32_t duty = args[ARG_duty_u16].u_int;
    if (duty >= 0) {
        self->duty_u16 = duty;
        self->duty_ns = VALUE_NOT_SET;
    }
    // Set duty_ns value?
    duty = args[ARG_duty_ns].u_int;
    if (duty >= 0) {
        self->duty_ns = duty;
        self->duty_u16 = VALUE_NOT_SET;
    }

    self->flags = 0;
    if (args[ARG_invert].u_int >= 0) {
        self->flags |= PWM_POLARITY_INVERTED;
    }

    configure_pwm(self);
}

// PWM(pin-tuple, freq, [args])
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw,
    const mp_obj_t *args) {
    mp_obj_t *items;
    uint32_t wanted_chan;
    const struct device *wanted_pwm;

    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get referred Pin object(s)
    if (!mp_obj_is_type(args[0], &mp_type_tuple)) {
        mp_raise_ValueError(
            MP_ERROR_TEXT("Pin id must be tuple of (\"pwm_x\", channel#)"));
    }

    mp_obj_get_array_fixed_n(args[0], 2, &items);
    wanted_pwm = zephyr_device_find(items[0]);
    wanted_chan = mp_obj_get_int(items[1]);

    machine_pwm_obj_t *pwm = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    pwm->pwm = wanted_pwm;
    pwm->channel = wanted_chan;
    pwm->duty_ns = VALUE_NOT_SET;
    pwm->duty_u16 = VALUE_NOT_SET;
    pwm->freq = VALUE_NOT_SET;
    pwm->flags = 0;

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        mp_machine_pwm_init_helper(pwm, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pwm;
}

// This called from pwm.deinit() method
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    self->duty_ns = 0;
    self->duty_u16 = VALUE_NOT_SET;
    configure_pwm(self);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->freq);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    self->freq = freq;
    configure_pwm(self);
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self,
    mp_int_t duty_ns) {
    self->duty_ns = duty_ns;
    self->duty_u16 = VALUE_NOT_SET;
    configure_pwm(self);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_ns);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_u16);
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self,
    mp_int_t duty_u16) {
    self->duty_ns = VALUE_NOT_SET;
    self->duty_u16 = duty_u16;
    configure_pwm(self);
}
