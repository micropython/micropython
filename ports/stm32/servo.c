/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"
#include "timer.h"
#include "servo.h"

#if MICROPY_HW_ENABLE_SERVO

// This file implements the pyb.Servo class which controls standard hobby servo
// motors that have 3-wires (ground, power, signal).
//
// The driver uses hardware PWM to drive servos on pins X1, X2, X3, X4 which are
// assumed to be on PA0, PA1, PA2, PA3 but not necessarily in that order (the
// pins PA0-PA3 are used directly if the X pins are not defined).
//
// TIM2 and TIM5 have CH1-CH4 on PA0-PA3 respectively.  They are both 32-bit
// counters with 16-bit prescaler.  TIM5 is used by this driver.

#define PYB_SERVO_NUM (4)

typedef struct _pyb_servo_obj_t {
    mp_obj_base_t base;
    const machine_pin_obj_t *pin;
    uint8_t pulse_min;          // units of 10us
    uint8_t pulse_max;          // units of 10us
    uint8_t pulse_centre;       // units of 10us
    uint8_t pulse_angle_90;     // units of 10us; pulse at 90 degrees, minus pulse_centre
    uint8_t pulse_speed_100;    // units of 10us; pulse at 100% forward speed, minus pulse_centre
    uint16_t pulse_cur;         // units of 10us
    uint16_t pulse_dest;        // units of 10us
    int16_t pulse_accum;
    uint16_t time_left;
} pyb_servo_obj_t;

static pyb_servo_obj_t pyb_servo_obj[PYB_SERVO_NUM];

void servo_init(void) {
    // reset servo objects
    for (int i = 0; i < PYB_SERVO_NUM; i++) {
        pyb_servo_obj[i].base.type = &pyb_servo_type;
        pyb_servo_obj[i].pulse_min = 64;
        pyb_servo_obj[i].pulse_max = 242;
        pyb_servo_obj[i].pulse_centre = 150;
        pyb_servo_obj[i].pulse_angle_90 = 97;
        pyb_servo_obj[i].pulse_speed_100 = 70;
        pyb_servo_obj[i].pulse_cur = 150;
        pyb_servo_obj[i].pulse_dest = 0;
        pyb_servo_obj[i].time_left = 0;
    }

    // assign servo objects to specific pins (must be some permutation of PA0-PA3)
    #ifdef pyb_pin_X1
    pyb_servo_obj[0].pin = pyb_pin_X1;
    pyb_servo_obj[1].pin = pyb_pin_X2;
    pyb_servo_obj[2].pin = pyb_pin_X3;
    pyb_servo_obj[3].pin = pyb_pin_X4;
    #else
    pyb_servo_obj[0].pin = pin_A0;
    pyb_servo_obj[1].pin = pin_A1;
    pyb_servo_obj[2].pin = pin_A2;
    pyb_servo_obj[3].pin = pin_A3;
    #endif
}

void servo_timer_irq_callback(void) {
    bool need_it = false;
    for (int i = 0; i < PYB_SERVO_NUM; i++) {
        pyb_servo_obj_t *s = &pyb_servo_obj[i];
        if (s->pulse_cur != s->pulse_dest) {
            // clamp pulse to within min/max
            if (s->pulse_dest < s->pulse_min) {
                s->pulse_dest = s->pulse_min;
            } else if (s->pulse_dest > s->pulse_max) {
                s->pulse_dest = s->pulse_max;
            }
            // adjust cur to get closer to dest
            if (s->time_left <= 1) {
                s->pulse_cur = s->pulse_dest;
                s->time_left = 0;
            } else {
                s->pulse_accum += s->pulse_dest - s->pulse_cur;
                s->pulse_cur += s->pulse_accum / s->time_left;
                s->pulse_accum %= s->time_left;
                s->time_left--;
                need_it = true;
            }
            // set the pulse width
            *(&TIM5->CCR1 + s->pin->pin) = s->pulse_cur;
        }
    }
    if (need_it) {
        __HAL_TIM_ENABLE_IT(&TIM5_Handle, TIM_IT_UPDATE);
    } else {
        __HAL_TIM_DISABLE_IT(&TIM5_Handle, TIM_IT_UPDATE);
    }
}

static void servo_init_channel(pyb_servo_obj_t *s) {
    static const uint8_t channel_table[4] =
    {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4};
    uint32_t channel = channel_table[s->pin->pin];

    // GPIO configuration
    mp_hal_pin_config(s->pin, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF2_TIM5);

    if (__HAL_RCC_TIM5_IS_CLK_DISABLED()) {
        timer_tim5_init();
    }

    // PWM mode configuration
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = s->pulse_cur; // units of 10us
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&TIM5_Handle, &oc_init, channel);

    // start PWM
    HAL_TIM_PWM_Start(&TIM5_Handle, channel);
}

/******************************************************************************/
// MicroPython bindings

static mp_obj_t pyb_servo_set(mp_obj_t port, mp_obj_t value) {
    int p = mp_obj_get_int(port);
    int v = mp_obj_get_int(value);
    if (v < 50) {
        v = 50;
    }
    if (v > 250) {
        v = 250;
    }
    switch (p) {
        case 1:
            TIM5->CCR1 = v;
            break;
        case 2:
            TIM5->CCR2 = v;
            break;
        case 3:
            TIM5->CCR3 = v;
            break;
        case 4:
            TIM5->CCR4 = v;
            break;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_servo_set_obj, pyb_servo_set);

static mp_obj_t pyb_pwm_set(mp_obj_t period, mp_obj_t pulse) {
    int pe = mp_obj_get_int(period);
    int pu = mp_obj_get_int(pulse);
    TIM5->ARR = pe;
    TIM5->CCR3 = pu;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_pwm_set_obj, pyb_pwm_set);

static void pyb_servo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_servo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<Servo %u at %uus>", self - &pyb_servo_obj[0] + 1, 10 * self->pulse_cur);
}

/// \classmethod \constructor(id)
/// Create a servo object.  `id` is 1-4.
static mp_obj_t pyb_servo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get servo number
    mp_int_t servo_id = mp_obj_get_int(args[0]) - 1;

    // check servo number
    if (!(0 <= servo_id && servo_id < PYB_SERVO_NUM)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Servo(%d) doesn't exist"), servo_id + 1);
    }

    // get and init servo object
    pyb_servo_obj_t *s = &pyb_servo_obj[servo_id];
    s->pulse_dest = s->pulse_cur;
    s->time_left = 0;
    servo_init_channel(s);

    return MP_OBJ_FROM_PTR(s);
}

/// \method pulse_width([value])
/// Get or set the pulse width in milliseconds.
static mp_obj_t pyb_servo_pulse_width(size_t n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get pulse width, in us
        return mp_obj_new_int(10 * self->pulse_cur);
    } else {
        // set pulse width, in us
        self->pulse_dest = mp_obj_get_int(args[1]) / 10;
        self->time_left = 0;
        servo_timer_irq_callback();
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_servo_pulse_width_obj, 1, 2, pyb_servo_pulse_width);

/// \method calibration([pulse_min, pulse_max, pulse_centre, [pulse_angle_90, pulse_speed_100]])
/// Get or set the calibration of the servo timing.
// TODO should accept 1 arg, a 5-tuple of values to set
static mp_obj_t pyb_servo_calibration(size_t n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get calibration values
        mp_obj_t tuple[5];
        tuple[0] = mp_obj_new_int(10 * self->pulse_min);
        tuple[1] = mp_obj_new_int(10 * self->pulse_max);
        tuple[2] = mp_obj_new_int(10 * self->pulse_centre);
        tuple[3] = mp_obj_new_int(10 * (self->pulse_angle_90 + self->pulse_centre));
        tuple[4] = mp_obj_new_int(10 * (self->pulse_speed_100 + self->pulse_centre));
        return mp_obj_new_tuple(5, tuple);
    } else if (n_args >= 4) {
        // set min, max, centre
        self->pulse_min = mp_obj_get_int(args[1]) / 10;
        self->pulse_max = mp_obj_get_int(args[2]) / 10;
        self->pulse_centre = mp_obj_get_int(args[3]) / 10;
        if (n_args == 4) {
            return mp_const_none;
        } else if (n_args == 6) {
            self->pulse_angle_90 = mp_obj_get_int(args[4]) / 10 - self->pulse_centre;
            self->pulse_speed_100 = mp_obj_get_int(args[5]) / 10 - self->pulse_centre;
            return mp_const_none;
        }
    }

    // bad number of arguments
    mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("calibration expecting 1, 4 or 6 arguments, got %d"), n_args);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_servo_calibration_obj, 1, 6, pyb_servo_calibration);

/// \method angle([angle, time=0])
/// Get or set the angle of the servo.
///
///   - `angle` is the angle to move to in degrees.
///   - `time` is the number of milliseconds to take to get to the specified angle.
static mp_obj_t pyb_servo_angle(size_t n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get angle
        return mp_obj_new_int((self->pulse_cur - self->pulse_centre) * 90 / self->pulse_angle_90);
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        self->pulse_dest = self->pulse_centre + (int16_t)((mp_float_t)self->pulse_angle_90 * mp_obj_get_float(args[1]) / MICROPY_FLOAT_CONST(90.0));
        #else
        self->pulse_dest = self->pulse_centre + self->pulse_angle_90 * mp_obj_get_int(args[1]) / 90;
        #endif
        if (n_args == 2) {
            // set angle immediately
            self->time_left = 0;
        } else {
            // set angle over a given time (given in milli seconds)
            self->time_left = mp_obj_get_int(args[2]) / 20;
            self->pulse_accum = 0;
        }
        servo_timer_irq_callback();
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_servo_angle_obj, 1, 3, pyb_servo_angle);

/// \method speed([speed, time=0])
/// Get or set the speed of a continuous rotation servo.
///
///   - `speed` is the speed to move to change to, between -100 and 100.
///   - `time` is the number of milliseconds to take to get to the specified speed.
static mp_obj_t pyb_servo_speed(size_t n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get speed
        return mp_obj_new_int((self->pulse_cur - self->pulse_centre) * 100 / self->pulse_speed_100);
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        self->pulse_dest = self->pulse_centre + (int16_t)((mp_float_t)self->pulse_speed_100 * mp_obj_get_float(args[1]) / MICROPY_FLOAT_CONST(100.0));
        #else
        self->pulse_dest = self->pulse_centre + self->pulse_speed_100 * mp_obj_get_int(args[1]) / 100;
        #endif
        if (n_args == 2) {
            // set speed immediately
            self->time_left = 0;
        } else {
            // set speed over a given time (given in milli seconds)
            self->time_left = mp_obj_get_int(args[2]) / 20;
            self->pulse_accum = 0;
        }
        servo_timer_irq_callback();
        return mp_const_none;
    }
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_servo_speed_obj, 1, 3, pyb_servo_speed);

static const mp_rom_map_elem_t pyb_servo_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pulse_width), MP_ROM_PTR(&pyb_servo_pulse_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&pyb_servo_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_angle), MP_ROM_PTR(&pyb_servo_angle_obj) },
    { MP_ROM_QSTR(MP_QSTR_speed), MP_ROM_PTR(&pyb_servo_speed_obj) },
};

static MP_DEFINE_CONST_DICT(pyb_servo_locals_dict, pyb_servo_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_servo_type,
    MP_QSTR_Servo,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_servo_make_new,
    print, pyb_servo_print,
    locals_dict, &pyb_servo_locals_dict
    );

#endif // MICROPY_HW_ENABLE_SERVO
