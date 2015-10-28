/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t duty;
} pyb_pwm_obj_t;

STATIC pyb_pwm_obj_t pyb_pwm_obj[] = {
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}},
    {{&pyb_pwm_type}}
};

#define NUM_PWM_OUT MP_ARRAY_SIZE(pyb_pwm_obj)
#define PWM_ID(obj) ((obj) - &pyb_pwm_obj[0])

void pyb_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_pwm_obj_t *self = self_in;
    mp_printf(print, "PWM Out(%u)", PWM_ID(self));
}

STATIC mp_obj_t pyb_pwm_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t pwm_id = mp_obj_get_int(args[0]);

    if (pwm_id<0 || pwm_id>=NUM_PWM_OUT) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "PWM Out %d does not exist", pwm_id));
    }
    mp_hal_configurePWMOut(pwm_id);
    return (mp_obj_t)&pyb_pwm_obj[pwm_id];
}

/// \method duty_cycle([value])
/// Get or set the duty cycle:
///
///   - With no argument, return duty cycle (0 to 100).
///   - With `value` given, set the duty cycle  (0 to 100)
STATIC mp_obj_t pyb_pwm_duty_cycle(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_pwm_obj_t *self = args[0];

    int numOut = PWM_ID(self);
    if(numOut<0 || numOut>=NUM_PWM_OUT)
    {
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "PWM Out %d does not exist", numOut));
	return mp_const_none;
    }

    if (n_args == 1) {
        // get duty cycle
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else {
        // set duty cycle
	uint32_t duty = (uint8_t)mp_obj_get_int(args[1]);
	if(duty>100)
	{
	        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "PWM Duty Cycle out of range"));
	        return mp_const_none;
	}
	mp_hal_setPWMDutyCycle(numOut,duty);
	self->duty=duty;
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_duty_cycle_obj, 1, 2, pyb_pwm_duty_cycle);



/// \static method set_frequency()
STATIC mp_obj_t pyb_pwm_set_freq(mp_obj_t freqObj) {

	uint32_t freq = mp_obj_get_int(freqObj);
	mp_hal_setPWMFequency(freq);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pwm_set_freq_obj, pyb_pwm_set_freq);





STATIC const mp_map_elem_t pyb_pwm_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_duty_cycle), (mp_obj_t)&pyb_pwm_duty_cycle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_frequency), (mp_obj_t)&pyb_pwm_set_freq_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_pwm_locals_dict, pyb_pwm_locals_dict_table);

const mp_obj_type_t pyb_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = pyb_pwm_print,
    .make_new = pyb_pwm_make_new,
    .locals_dict = (mp_obj_t)&pyb_pwm_locals_dict,
};
