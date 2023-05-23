#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

// port-specific includes
#include "drivers/machine/psoc6_gpio.h"
#include "drivers/machine/psoc6_pwm.h"
#include "mplogger.h"
#include "pins.h"

extern mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);

#define VALUE_NOT_SET -1

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "frequency=%u duty_cycle=%f invert=%u>", self->fz, self->duty_us, self->invert);
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

    if ((args[ARG_freq].u_int != VALUE_NOT_SET)) {
        pwm_freq_duty_set(&self->pwm_obj, args[ARG_freq].u_int, self->duty_us);
        self->fz = args[ARG_freq].u_int;

    }

    if ((args[ARG_duty_u16].u_int != VALUE_NOT_SET)) {
        pwm_freq_duty_set(&self->pwm_obj, self->fz, args[ARG_duty_u16].u_int * 100 / 65536);
        self->duty_us = args[ARG_duty_u16].u_int * 100 / 65536;
    }

    if (args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        pwm_duty_set_ns(&self->pwm_obj, self->fz, args[ARG_duty_ns].u_int);
        self->duty_ns = args[ARG_duty_ns].u_int;
    }

    if (args[ARG_invert].u_int != VALUE_NOT_SET) {
        self->invert = !!args[ARG_invert].u_int;
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

    // Initialize PWM
    cy_rslt_t result = pwm_init(self);

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
}

STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    // How to check whether PWM is active?
    return MP_OBJ_NEW_SMALL_INT(self->fz);

}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    self->fz = freq;
    pwm_freq_duty_set(&self->pwm_obj, freq, self->duty_us);
}

STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_us);
}

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    if (duty_u16 > 65535) {
        duty_u16 = 65535;
    }
    self->duty_us = duty_u16 * 100 / 65536;
    pwm_freq_duty_set(&self->pwm_obj, self->fz, duty_u16 * 100 / 65536);
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_ns);
}

STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    self->duty_ns = duty_ns;
    pwm_freq_duty_set(&self->pwm_obj, self->fz, duty_ns);
}
