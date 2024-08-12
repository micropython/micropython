/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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
#include "py/mphal.h"
#include "modmachine.h"

// port-specific includes
#include "machine_pin_phy.h"
#include "mplogger.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    cyhal_pwm_t pwm_obj;
    uint32_t pin;
    uint32_t fz;
    uint8_t duty_type;
    mp_int_t duty;
    // bool invert;
} machine_pwm_obj_t;

static machine_pwm_obj_t *pwm_obj[MAX_PWM_OBJS] = { NULL };

static inline machine_pwm_obj_t *pwm_obj_alloc() {
    for (uint8_t i = 0; i < MAX_PWM_OBJS; i++)
    {
        if (pwm_obj[i] == NULL) {
            pwm_obj[i] = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
            return pwm_obj[i];
        }
    }

    return NULL;
}

static inline void pwm_obj_free(machine_pwm_obj_t *pwm_obj_ptr) {
    for (uint8_t i = 0; i < MAX_PWM_OBJS; i++)
    {
        if (pwm_obj[i] == pwm_obj_ptr) {
            pwm_obj[i] = NULL;
        }
    }
}

enum {
    VALUE_NOT_SET = -1,
    DUTY_NOT_SET = 0,
    DUTY_U16,
    DUTY_NS
};

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);

static cy_rslt_t pwm_freq_duty_set(cyhal_pwm_t *pwm_obj, uint32_t fz, float duty_cycle) {
    return cyhal_pwm_set_duty_cycle(pwm_obj, duty_cycle * 100, fz); // duty_cycle in percentage
}

static inline cy_rslt_t pwm_duty_set_ns(cyhal_pwm_t *pwm_obj, uint32_t fz, uint32_t pulse_width) {
    return cyhal_pwm_set_period(pwm_obj, 1000000 / fz, pulse_width / 1000); // !# * --> /
}

/*STATIC inline cy_rslt_t pwm_advanced_init(machine_pwm_obj_t *machine_pwm_obj) {
    return cyhal_pwm_init_adv(&machine_pwm_obj->pwm_obj, machine_pwm_obj->pin, NC, CYHAL_PWM_LEFT_ALIGN, true, 0, true, NULL); // complimentary pin set as not connected
}*/

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "frequency=%u duty_cycle=%f", self->fz, (double)self->duty);
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns};
    // enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        // { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // self->active = 1;

    if ((args[ARG_freq].u_int != VALUE_NOT_SET)) {
        // pwm_freq_duty_set(&self->pwm_obj, args[ARG_freq].u_int, self->duty);
        self->fz = args[ARG_freq].u_int;
    }

    if ((args[ARG_duty_u16].u_int != VALUE_NOT_SET)) {
        float val = (float)(args[ARG_duty_u16].u_int) / (float)65535;
        pwm_freq_duty_set(&self->pwm_obj, self->fz, val);
        self->duty = args[ARG_duty_u16].u_int;
        self->duty_type = DUTY_U16;
    }

    if (args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        pwm_duty_set_ns(&self->pwm_obj, self->fz, args[ARG_duty_ns].u_int);
        self->duty = args[ARG_duty_ns].u_int;
        self->duty_type = DUTY_NS;
    }

    // inverts the respective output if the value is True
    /*if (args[ARG_invert].u_int != VALUE_NOT_SET) {
        self->invert = args[ARG_invert].u_int;
        if (self->invert == 1) {
            cyhal_pwm_free(&self->pwm_obj);
            cy_rslt_t result = pwm_advanced_init(self);
            if (result != CY_RSLT_SUCCESS) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM initialisation failed with return code %lx ! and invert output is not available"), result);
            }
            self->duty_type = DUTY_U16;
            self->duty = ((1) - ((self->duty) / 65535)) * 65535;
        }
    }*/
    cyhal_pwm_start(&self->pwm_obj);
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get static peripheral object.
    machine_pwm_obj_t *self = pwm_obj_alloc();
    self->pin = pin_addr_by_name(all_args[0]);
    self->duty_type = DUTY_NOT_SET;
    self->fz = -1;
    // self->invert = -1;

    // Initialize PWM
    cy_rslt_t result = cyhal_pwm_init(&self->pwm_obj, self->pin, NULL);

    // To check whether PWM init is successful
    if (result != CY_RSLT_SUCCESS) {
        assert_pin_phy_used(result);
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM initialisation failed with return code %lx !"), result);
    }

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, all_args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    cyhal_pwm_stop(&self->pwm_obj);
    cyhal_pwm_free(&self->pwm_obj);
    pwm_obj_free(self);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_NS) {
        // duty_cycle = pulsewidth(ns)*freq(hz);
        return mp_obj_new_float(((self->duty) * (self->fz) * 65535) / 1000000000 - 1);
    } else {
        return mp_obj_new_float(self->duty);
    }
}

// sets the duty cycle as a ratio duty_u16 / 65535.
static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    // Check the value is more than the max value
    self->duty = duty_u16 > 65535 ? 65535 : duty_u16;
    self->duty_type = DUTY_U16;
    pwm_freq_duty_set(&self->pwm_obj, self->fz, (float)(self->duty) / (float)65535); // s conversion of duty_u16 into dutyu16/65535
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_U16) {
        return mp_obj_new_float(((self->duty) * 1000000000) / ((self->fz) * 65535));   // pw (ns) = duty_cycle*10^9/fz
    } else {
        return mp_obj_new_float(self->duty);
    }
}

// sets the pulse width in nanoseconds
static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    self->duty = duty_ns;
    self->duty_type = DUTY_NS;
    pwm_duty_set_ns(&self->pwm_obj, self->fz, duty_ns);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->fz);

}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    self->fz = freq;
    pwm_freq_duty_set(&self->pwm_obj, freq, self->duty);
    if (self->duty_type == DUTY_NS) {
        self->duty = ((self->duty) * (self->fz) * 65535) / 1000000000;
        mp_machine_pwm_duty_set_ns(self, self->duty);
    }
}

void mod_pwm_deinit() {
    for (uint8_t i = 0; i < MAX_PWM_OBJS; i++) {
        if (pwm_obj[i] != NULL) {
            mp_machine_pwm_deinit(pwm_obj[i]);
        }
    }
}
