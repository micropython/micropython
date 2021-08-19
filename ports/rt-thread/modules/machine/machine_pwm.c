/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 ChenYong (chenyong@rt-thread.com)
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

#ifdef MICROPYTHON_USING_MACHINE_PWM

#include <rtthread.h>
#include <drivers/rt_drv_pwm.h>

#define MP_PWM_PULSE_MAX               255
#define MP_PWM_PERIOD_GET(freq)        (1000000000 / (freq))
#define MP_PWM_PULSE_GET(period, duty) ((period) / MP_PWM_PULSE_MAX * (duty))

extern const mp_obj_type_t machine_pwm_type;

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    struct rt_device_pwm *pwm_device;
    char dev_name[RT_NAME_MAX];
    uint8_t is_init;
    int8_t id;
    uint8_t channel;
    uint8_t duty;
    uint32_t freq;
} machine_pwm_obj_t;

STATIC void machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = self_in;

    mp_printf(print, "PWM(%p; ", self);
    if (self->id >= 0) {
        mp_printf(print, "pwm_id=%d, ", self->id);
    } else {
        mp_printf(print, "pwm_name=%s, ", self->dev_name);
    }
    mp_printf(print, "channel=%d, ", self->channel);
    mp_printf(print, "freq=%d, ", self->freq);
    mp_printf(print, "duty=%d)", self->duty);
}

STATIC void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, msg));
    }
}

STATIC void machine_pwm_init_helper(machine_pwm_obj_t *self,
                                 size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rt_err_t result = RT_EOK;
    uint32_t period = 0, pulse = 0;
    char pwm_dev_name[RT_NAME_MAX];
    struct rt_device_pwm *pwm_device = RT_NULL;
    enum { ARG_channel, ARG_freq, ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_channel,  MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int tval = args[ARG_channel].u_int;
    if ((tval < 0) || (tval > 4)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad channel %d", tval));
    }
    self->channel = tval;

    tval = args[ARG_freq].u_int;
    if ((tval < 1) || (tval > 156250)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad frequency %d", tval));
    }
    self->freq = tval;

    tval = args[ARG_duty].u_int;
    if ((tval < 0) || (tval > 255)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad duty %d", tval));
    }
    self->duty = tval;

    if (self->id >= 0) {
        rt_snprintf(pwm_dev_name, sizeof(pwm_dev_name), "pwm%d", self->id);
    } else {
        rt_strncpy(pwm_dev_name, self->dev_name, RT_NAME_MAX);
    }

    pwm_device = (struct rt_device_pwm *) rt_device_find(pwm_dev_name);
    if (pwm_device == RT_NULL || pwm_device->parent.type != RT_Device_Class_Miscellaneous) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "PWM(%s) don't exist", pwm_dev_name));
    }
    self->pwm_device = pwm_device;

    // get period number by frequency
    period = MP_PWM_PERIOD_GET(self->freq);
    // get pulse number by duty
    pulse = MP_PWM_PULSE_GET(period, self->duty);

    result = rt_pwm_set(pwm_device, self->channel, period, pulse);
    error_check(result == RT_EOK, "PWM set information error");

    result = rt_pwm_enable(pwm_device, self->channel);
    error_check(result == RT_EOK, "PWM enable error");

    self->is_init = RT_TRUE;
}

STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type,
                                  size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // create PWM object from the given pin
    machine_pwm_obj_t *self = m_new_obj(machine_pwm_obj_t);
    self->base.type = &machine_pwm_type;
    self->is_init = RT_FALSE;

    // check input PWM device name or ID
    if (mp_obj_is_small_int(args[0])) {
        self->id = mp_obj_get_int(args[0]);
    } else if (mp_obj_is_qstr(args[0])) {
        self->id = -1;
        rt_strncpy(self->dev_name, mp_obj_str_get_str(args[0]), RT_NAME_MAX);
    } else {
        error_check(0, "Input PWM device name or ID error.");
    }

    self->channel = 0;
    self->freq = 1;
    self->duty = 0;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_pwm_init(size_t n_args,
                              const mp_obj_t *args, mp_map_t *kw_args) {
    machine_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pwm_init_obj, 1, machine_pwm_init);

STATIC mp_obj_t machine_pwm_deinit(mp_obj_t self_in) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rt_err_t result = RT_EOK;

    if (self->is_init == RT_TRUE) {
        result = rt_pwm_disable(self->pwm_device, self->channel);
        error_check(result == RT_EOK, "PWM disable error");
        self->is_init = RT_FALSE;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_deinit_obj, machine_pwm_deinit);

STATIC mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t period = 0, pulse = 0;
    rt_err_t result = RT_EOK;

    error_check(self->is_init == RT_TRUE, "PWM device uninitialized");

    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    }

    // set
    int tval = mp_obj_get_int(args[1]);
    if ((tval < 1) || (tval > 156250)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad frequency %d", tval));
    }

    // get period number by frequency
    period = MP_PWM_PERIOD_GET(tval);
    // get pulse number by duty
    pulse = MP_PWM_PULSE_GET(period, self->duty);

    result = rt_pwm_set(self->pwm_device, self->channel, period, pulse);
    error_check(result == RT_EOK, "PWM set information error");
    self->freq = tval;

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freq_obj, 1, 2, machine_pwm_freq);

STATIC mp_obj_t machine_pwm_duty(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t period = 0, pulse = 0;
    rt_err_t result = RT_EOK;

    error_check(self->is_init == RT_TRUE, "PWM device uninitialized");

    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    }

    // set
    int tval = mp_obj_get_int(args[1]);
    if ((tval < 0) || (tval > 255)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad duty %d", tval));
    }

    // get period number by frequency
    period = MP_PWM_PERIOD_GET(self->freq);
    // get pulse number by duty
    pulse = MP_PWM_PULSE_GET(period, tval);

    result = rt_pwm_set(self->pwm_device, self->channel, period, pulse);
    error_check(result == RT_EOK, "PWM set information error");
    self->duty = tval;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_obj,
        1, 2, machine_pwm_duty);

STATIC const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&machine_pwm_duty_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict,
                            machine_pwm_locals_dict_table);

const mp_obj_type_t machine_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = machine_pwm_print,
    .make_new = machine_pwm_make_new,
    .locals_dict = (mp_obj_dict_t *) &machine_pwm_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_PWM
