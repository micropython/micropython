/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

#include "py/runtime.h"
#include "py/mphal.h"

// PIC32MX Output Compare (PWM) base addresses
#define OC1_BASE  0xBF803000
#define OC2_BASE  0xBF803200
#define OC3_BASE  0xBF803400
#define OC4_BASE  0xBF803600
#define OC5_BASE  0xBF803800

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t pwm_id;
    uint32_t freq;
    uint16_t duty;
    uint8_t pin;
} machine_pwm_obj_t;

STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, true);

    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, type);
    self->pin = mp_obj_get_int(args[0]);
    self->freq = (n_args >= 2) ? mp_obj_get_int(args[1]) : 1000;
    self->duty = (n_args >= 3) ? mp_obj_get_int(args[2]) : 512;

    // TODO: Configure Output Compare module for PWM

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    } else {
        self->freq = mp_obj_get_int(args[1]);
        // TODO: Reconfigure PWM frequency
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freq_obj, 1, 2, machine_pwm_freq);

STATIC mp_obj_t machine_pwm_duty(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else {
        self->duty = mp_obj_get_int(args[1]);
        // TODO: Update PWM duty cycle
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_obj, 1, 2, machine_pwm_duty);

STATIC const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&machine_pwm_duty_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pwm_type,
    MP_QSTR_PWM,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pwm_make_new,
    locals_dict, &machine_pwm_locals_dict
);
