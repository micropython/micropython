#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

// port-specific includes
#include "drivers/machine/psoc6_gpio.h"
#include "drivers/machine/psoc6_pwm.h"
#include "mplogger.h"
#include "pins.h"

extern mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);

enum {
    VALUE_NOT_SET = -1,
    DUTY_NOT_SET = 0,
    DUTY_U16,
    DUTY_NS
};

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);
STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_float_t duty_u16);
STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_float_t duty_ns);

// To check whether the PWM is active
STATIC void pwm_is_active(machine_pwm_obj_t *self) {
    if (self->active == 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("PWM inactive"));
    }
}

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    pwm_is_active(self_in);
    mp_printf(print, "frequency=%u duty_cycle=%f invert=%u", self->fz, self->duty, self->invert);
}

STATIC void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    self->active = 1;

    if ((args[ARG_freq].u_int != VALUE_NOT_SET)) {
        pwm_freq_duty_set(&self->pwm_obj, args[ARG_freq].u_int, self->duty);
        self->fz = args[ARG_freq].u_int;
    }

    if ((args[ARG_duty_u16].u_int != VALUE_NOT_SET)) {
        pwm_freq_duty_set(&self->pwm_obj, self->fz, args[ARG_duty_u16].u_int / 65535);
        self->duty = args[ARG_duty_u16].u_int;
        self->duty_type = DUTY_U16;
    }

    if (args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        pwm_duty_set_ns(&self->pwm_obj, self->fz, args[ARG_duty_ns].u_int);
        self->duty = args[ARG_duty_ns].u_int;
        self->duty_type = DUTY_NS;
    }

    // inverts the respective output if the value is True
    if (args[ARG_invert].u_int != VALUE_NOT_SET) {
        self->invert = args[ARG_invert].u_int;
        if (self->invert == 1) {
            pwm_deinit(&self->pwm_obj);
            cy_rslt_t result = pwm_advanced_init(self);
            if (result != CY_RSLT_SUCCESS) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM initialisation failed with return code %lx ! and invert output is not available"), result);
            }
            self->duty_type = DUTY_U16;
            self->duty = ((1) - ((self->duty) / 65535)) * 65535;
        }
    }
    pwm_start(&self->pwm_obj);
}

STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get GPIO to connect to PWM.
    int pin = mp_hal_get_pin_obj(all_args[0]);

    // Get static peripheral object.
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->base.type = &machine_pwm_type;
    self->pin = pin;
    self->active = 0;
    self->duty_type = DUTY_NOT_SET;
    self->fz = -1;
    self->invert = -1;

    // Initialize PWM
    cy_rslt_t result = pwm_init(self);
    // To check whether PWM init is successfull
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM initialisation failed with return code %lx !"), result);
    }

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, all_args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    pwm_deinit(&self->pwm_obj);
    self->active = 0;
}


STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    return MP_OBJ_NEW_SMALL_INT(self->fz);

}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    pwm_is_active(self);
    self->fz = freq;
    pwm_freq_duty_set(&self->pwm_obj, freq, self->duty);
    if (self->duty_type == DUTY_NS) {
        self->duty = ((self->duty) * (self->fz) * 65535) / 1000000000;
        mp_machine_pwm_duty_set_ns(self, self->duty);
    }
}

STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    if (self->duty_type == DUTY_NS) {
        // duty_cycle = pulsewidth(ns)*freq(hz);
        return mp_obj_new_float(((self->duty) * (self->fz) * 65535) / 1000000000);
    } else {
        return mp_obj_new_float(self->duty);
    }
}

// sets the duty cycle as a ratio duty_u16 / 65535.
STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_float_t duty_u16) {
    pwm_is_active(self);
    // Check the value is more than the max value
    if (duty_u16 > 65535) {
        duty_u16 = 65535;
    }
    self->duty = duty_u16;
    self->duty_type = DUTY_U16;
    pwm_freq_duty_set(&self->pwm_obj, self->fz, (self->duty) / 65535); // conversion of duty_u16 into dutyu16/65535
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    if (self->duty_type == DUTY_U16) {
        return mp_obj_new_float(((self->duty) * 1000000000) / ((self->fz) * 65535));   // pw (ns) = duty_cycle*10^9/fz
    } else {
        return mp_obj_new_float(self->duty);
    }
}

// sets the pulse width in nanoseconds
STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_float_t duty_ns) {
    pwm_is_active(self);
    self->duty = duty_ns;
    self->duty_type = DUTY_NS;
    pwm_freq_duty_set(&self->pwm_obj, self->fz, duty_ns);
}
