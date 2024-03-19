/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include "py/mphal.h"
#include "py/mperrno.h"

#include "drv_pwm.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;

    uint8_t instance;
    uint8_t channel;

    drv_pwm_t drv;
} machine_pwm_obj_t;

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint32_t freq;
    uint16_t duty;

    drv_pwm_freq_get(&self->drv, &freq);
    drv_pwm_duty_get(&self->drv, &duty);

    uint32_t duty_ns = (uint64_t)duty * 1000000000 / 65536 / freq;

    mp_printf(print, "PWM(%d, ch=%d, ", self->instance, self->channel);
    mp_printf(print, "freq=%d, duty_u16=%d, duty_ns=%d", freq, duty, duty_ns);
    mp_printf(print, ")");
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args, args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    /* Differences from RA port: freq is a kw_arg (compatible with machine.PWM class). */

    enum { ARG_pin, ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,      MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_pin].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin does not exists."));
    } else {
        mp_hal_pin_obj_t pin = args[ARG_pin].u_obj;

        const machine_pin_af_t *af = mp_hal_pin_find_pwm(pin);
        if (af == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("pin is not PWM capable."));
        }

        self->instance = af->af_instance_id;
        self->channel = af->af_attribute;

        mp_hal_pin_af(pin, af->af_id);
    }

    if (args[ARG_freq].u_int == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq is not provided."));
    }

    bool inverted = args[ARG_invert].u_int ? true : false;

    int ret = drv_pwm_init(&self->drv, self->instance, self->channel, args[ARG_freq].u_int, inverted);
    if (ret < 0) {
        if (ret == -EBUSY) {
            /* No free channel for the PWM instance */
            mp_raise_ValueError(MP_ERROR_TEXT("channel is not available"));
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("channel initialization failed"));
        }
    }

    if ((args[ARG_duty_u16].u_int == -1) && (args[ARG_duty_ns].u_int == -1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("one of the duty options must be provided"));
    }

    if ((args[ARG_duty_u16].u_int != -1) && (args[ARG_duty_ns].u_int != -1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("only one of the duty options can be provided"));
    }

    if (args[ARG_duty_u16].u_int != -1) {
        drv_pwm_duty_set(&self->drv, args[ARG_duty_u16].u_int);
    }

    if (args[ARG_duty_ns].u_int != -1) {
        uint16_t duty_u16 = (args[ARG_duty_ns].u_int * 65536 * args[ARG_freq].u_int) / 1000000000;
        drv_pwm_duty_set(&self->drv, duty_u16);
    }

    drv_pwm_start(&self->drv);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    drv_pwm_deinit(&self->drv);

    /* TODO: Do we need to configure the IO to its previous state? */
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    uint32_t freq;

    drv_pwm_freq_get(&self->drv, &freq);

    return MP_OBJ_NEW_SMALL_INT(freq);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    drv_pwm_freq_set(&self->drv, freq);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    uint16_t duty_u16;
    drv_pwm_duty_get(&self->drv, &duty_u16);
    return MP_OBJ_NEW_SMALL_INT(duty_u16);
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    drv_pwm_duty_set(&self->drv, duty_u16);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(0);
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    uint32_t freq;

    drv_pwm_freq_get(&self->drv, &freq);

    uint64_t duty = ((uint64_t)duty_ns * freq * 65536) / 1000000000;

    drv_pwm_duty_set(&self->drv, duty);
}
