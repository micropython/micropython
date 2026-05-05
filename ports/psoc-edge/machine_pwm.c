/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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
#include "cy_tcpwm_pwm.h"
#include "cycfg_peripherals.h"

typedef enum {
    VALUE_NOT_SET = -1,
    DUTY_NOT_SET = 0,
    DUTY_U16,
    DUTY_NS
} duty_type_t;

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    cy_stc_tcpwm_pwm_config_t pwm_obj;
    uint32_t pin;
    uint32_t frequency;
    duty_type_t duty_type;
    mp_int_t duty;
    bool invert;
} machine_pwm_obj_t;

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "PWM() - not implemented");
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));   

}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM partially implemented"));
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}


