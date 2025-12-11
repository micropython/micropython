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

 #define pwm_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    cyhal_pwm_t pwm_obj;
    uint32_t pin;
    uint32_t fz;
    uint8_t duty_type;
    mp_int_t duty;
} machine_pwm_obj_t;

static machine_pwm_obj_t *pwm_obj[MAX_PWM_OBJS] = { NULL };

static inline machine_pwm_obj_t *pwm_obj_alloc() {
    for (uint8_t i = 0; i < MAX_PWM_OBJS; i++) {
        if (pwm_obj[i] == NULL) {
            pwm_obj[i] = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
            return pwm_obj[i];
        }
    }

    return NULL;
}

static inline void pwm_obj_free(machine_pwm_obj_t *pwm_obj_ptr) {
    for (uint8_t i = 0; i < MAX_PWM_OBJS; i++) {
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

/* Unit conversion macros */
 #define pwm_duty_cycle_ns_to_u16(duty_ns, fz)       ((int)(((float)(duty_ns * fz) / (float)1000000000) * (float)65536) - 1)
 #define pwm_duty_cycle_u16_to_ns(duty_u16, fz)      ((int)(((float)(duty_u16 + 1) / (float)65536) * ((float)1000000000 / (float)fz)))
 #define pwm_duty_cycle_u16_to_percent(duty_u16)     ((float)((duty_u16) * 100) / (float)65535)
 #define pwm_freq_to_period_us(fz)                   ((uint32_t)(1000000 / fz))
 #define pwm_period_ns_to_us(period_ns)              ((uint32_t)(period_ns / 1000))

static void pwm_duty_ns_assert(mp_int_t duty_ns, uint32_t fz) {
    if (duty_ns > (int)(1000000000 / fz)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM duty in ns is larger than the period %d ns"), (int)(1000000000 / fz));
    }
}

static inline void pwm_freq_duty_set(cyhal_pwm_t *pwm_obj, uint32_t fz, mp_int_t duty_cycle) {
    cy_rslt_t ret = cyhal_pwm_set_duty_cycle(pwm_obj, pwm_duty_cycle_u16_to_percent(duty_cycle), fz);
    pwm_assert_raise_val("PWM frequency and duty cycle set failed with return code %lx !", ret);
}

static void pwm_duty_set_ns(cyhal_pwm_t *pwm_obj, uint32_t fz, uint32_t pulse_width) {
    cy_rslt_t ret = cyhal_pwm_set_period(pwm_obj, pwm_freq_to_period_us(fz), pwm_period_ns_to_us(pulse_width));
    pwm_assert_raise_val("PWM period set failed with return code %lx !", ret);
}

static void pwm_config(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_U16) {
        pwm_freq_duty_set(&self->pwm_obj, self->fz, self->duty);
    } else {
        pwm_duty_set_ns(&self->pwm_obj, self->fz, self->duty);
    }
}

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t pin = mp_obj_new_int(self->pin);
    const char *pin_name = mp_obj_str_get_str(pin_name_by_addr(pin));
    mp_printf(print, "PWM(\"%s\",freq=%u, duty_u16=%f)", pin_name, self->fz, (double)self->duty);
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns};

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if ((args[ARG_freq].u_int != VALUE_NOT_SET)) {
        self->fz = args[ARG_freq].u_int;
    }

    if ((args[ARG_duty_u16].u_int != VALUE_NOT_SET) &&
        (args[ARG_duty_ns].u_int != VALUE_NOT_SET)) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM duty should be specified only in one format"));
    }

    if ((args[ARG_duty_u16].u_int != VALUE_NOT_SET)) {
        self->duty = args[ARG_duty_u16].u_int > 65535 ? 65535 : args[ARG_duty_u16].u_int;
        self->duty_type = DUTY_U16;
    } else if (args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        pwm_duty_ns_assert(args[ARG_duty_ns].u_int, self->fz);
        self->duty = args[ARG_duty_ns].u_int;
        self->duty_type = DUTY_NS;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM duty should be specified in either ns or u16"));
    }

    pwm_config(self);

    cy_rslt_t result = cyhal_pwm_start(&self->pwm_obj);
    pwm_assert_raise_val("PWM start failed with return code %lx !", result);
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get static peripheral object.
    machine_pwm_obj_t *self = pwm_obj_alloc();
    self->pin = pin_addr_by_name(all_args[0]);
    self->duty_type = DUTY_NOT_SET;
    self->fz = -1;

    cy_rslt_t result = cyhal_pwm_init(&self->pwm_obj, self->pin, NULL);
    assert_pin_phy_used(result);
    pwm_assert_raise_val("PWM initialisation failed with return code %lx !", result);

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, all_args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    cy_rslt_t result = cyhal_pwm_stop(&self->pwm_obj);
    pwm_assert_raise_val("PWM stop failed with return code %lx !", result);
    cyhal_pwm_free(&self->pwm_obj);
    pwm_obj_free(self);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_NS) {
        return mp_obj_new_int(pwm_duty_cycle_ns_to_u16(self->duty, self->fz));
    } else {
        return mp_obj_new_int(self->duty);
    }
}

// sets the duty cycle as a ratio duty_u16 / 65535.
static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    // Check the value is more than the max value
    self->duty = duty_u16 > 65535 ? 65535 : duty_u16;
    self->duty_type = DUTY_U16;
    pwm_config(self);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_U16) {
        return mp_obj_new_int(pwm_duty_cycle_u16_to_ns(self->duty, self->fz));
    } else {
        return mp_obj_new_int(self->duty);
    }
}

// sets the pulse width in nanoseconds
static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    pwm_duty_ns_assert(duty_ns, self->fz);
    self->duty = duty_ns;
    self->duty_type = DUTY_NS;
    pwm_config(self);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->fz);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    if (self->duty_type == DUTY_NS) {
        pwm_duty_ns_assert(self->duty, freq);
    }
    self->fz = freq;
    pwm_config(self);
}

void machine_pwm_deinit_all() {
    for (uint8_t i = 0; i < MAX_PWM_OBJS; i++) {
        if (pwm_obj[i] != NULL) {
            mp_machine_pwm_deinit(pwm_obj[i]);
        }
    }
}
