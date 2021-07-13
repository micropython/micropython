/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
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

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class PWMOut:
//|     """Output a Pulse Width Modulated signal on a given pin."""
//|
//|     def __init__(self, pin: microcontroller.Pin, *, duty_cycle: int = 0, frequency: int = 500, variable_frequency: bool = False) -> None:
//|         """Create a PWM object associated with the given pin. This allows you to
//|         write PWM signals out on the given pin. Frequency is fixed after init
//|         unless ``variable_frequency`` is True.
//|
//|         .. note:: When ``variable_frequency`` is True, further PWM outputs may be
//|           limited because it may take more internal resources to be flexible. So,
//|           when outputting both fixed and flexible frequency signals construct the
//|           fixed outputs first.
//|
//|         :param ~microcontroller.Pin pin: The pin to output to
//|         :param int duty_cycle: The fraction of each pulse which is high. 16-bit
//|         :param int frequency: The target frequency in Hertz (32-bit)
//|         :param bool variable_frequency: True if the frequency will change over time
//|
//|         Simple LED fade::
//|
//|           import pwmio
//|           import board
//|
//|           pwm = pwmio.PWMOut(board.D13)     # output on D13
//|           pwm.duty_cycle = 2 ** 15            # Cycles the pin with 50% duty cycle (half of 2 ** 16) at the default 500hz
//|
//|         PWM at specific frequency (servos and motors)::
//|
//|           import pwmio
//|           import board
//|
//|           pwm = pwmio.PWMOut(board.D13, frequency=50)
//|           pwm.duty_cycle = 2 ** 15                  # Cycles the pin with 50% duty cycle (half of 2 ** 16) at 50hz
//|
//|         Variable frequency (usually tones)::
//|
//|           import pwmio
//|           import board
//|           import time
//|
//|           pwm = pwmio.PWMOut(board.D13, duty_cycle=2 ** 15, frequency=440, variable_frequency=True)
//|           time.sleep(0.2)
//|           pwm.frequency = 880
//|           time.sleep(0.1)"""
//|         ...
//|
STATIC mp_obj_t pwmio_pwmout_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    enum { ARG_pin, ARG_duty_cycle, ARG_frequency, ARG_variable_frequency };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_duty_cycle, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_frequency, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 500} },
        { MP_QSTR_variable_frequency, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(parsed_args[ARG_pin].u_obj);

    uint16_t duty_cycle = parsed_args[ARG_duty_cycle].u_int;
    uint32_t frequency = parsed_args[ARG_frequency].u_int;
    bool variable_frequency = parsed_args[ARG_variable_frequency].u_bool;

    // create PWM object from the given pin
    pwmio_pwmout_obj_t *self = m_new_obj(pwmio_pwmout_obj_t);
    self->base.type = &pwmio_pwmout_type;
    pwmout_result_t result = common_hal_pwmio_pwmout_construct(self, pin, duty_cycle, frequency, variable_frequency);
    switch (result) {
        case PWMOUT_OK:
            break;
        case PWMOUT_INVALID_PIN:
            mp_raise_ValueError(translate("Invalid pin"));
            break;
        case PWMOUT_INVALID_FREQUENCY:
            mp_raise_ValueError(translate("Invalid PWM frequency"));
            break;
        case PWMOUT_INVALID_FREQUENCY_ON_PIN:
            mp_raise_ValueError(translate("Frequency must match existing PWMOut using this timer"));
            break;
        case PWMOUT_VARIABLE_FREQUENCY_NOT_AVAILABLE:
            mp_raise_ValueError(translate("Cannot vary frequency on a timer that is already in use"));
            break;
        case PWMOUT_ALL_TIMERS_ON_PIN_IN_USE:
            mp_raise_ValueError(translate("All timers for this pin are in use"));
            break;
        case PWMOUT_ALL_TIMERS_IN_USE:
            mp_raise_RuntimeError(translate("All timers in use"));
            break;
        case PWMOUT_ALL_CHANNELS_IN_USE:
            mp_raise_RuntimeError(translate("All channels in use"));
            break;
        default:
        case PWMOUT_INITIALIZATION_ERROR:
            mp_raise_RuntimeError(translate("Could not start PWM"));
            break;
    }

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the PWMOut and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t pwmio_pwmout_deinit(mp_obj_t self_in) {
    pwmio_pwmout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_pwmio_pwmout_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pwmio_pwmout_deinit_obj, pwmio_pwmout_deinit);

STATIC void check_for_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> PWMOut:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t pwmio_pwmout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_pwmio_pwmout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pwmio_pwmout___exit___obj, 4, 4, pwmio_pwmout_obj___exit__);

//|     duty_cycle: int
//|     """16 bit value that dictates how much of one cycle is high (1) versus low
//|     (0). 0xffff will always be high, 0 will always be low and 0x7fff will
//|     be half high and then half low.
//|
//|     Depending on how PWM is implemented on a specific board, the internal
//|     representation for duty cycle might have less than 16 bits of resolution.
//|     Reading this property will return the value from the internal representation,
//|     so it may differ from the value set."""
//|
STATIC mp_obj_t pwmio_pwmout_obj_get_duty_cycle(mp_obj_t self_in) {
    pwmio_pwmout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_pwmio_pwmout_get_duty_cycle(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(pwmio_pwmout_get_duty_cycle_obj, pwmio_pwmout_obj_get_duty_cycle);

STATIC mp_obj_t pwmio_pwmout_obj_set_duty_cycle(mp_obj_t self_in, mp_obj_t duty_cycle) {
    pwmio_pwmout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    mp_int_t duty = mp_obj_get_int(duty_cycle);
    if (duty < 0 || duty > 0xffff) {
        mp_raise_ValueError(translate("PWM duty_cycle must be between 0 and 65535 inclusive (16 bit resolution)"));
    }
    common_hal_pwmio_pwmout_set_duty_cycle(self, duty);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pwmio_pwmout_set_duty_cycle_obj, pwmio_pwmout_obj_set_duty_cycle);

const mp_obj_property_t pwmio_pwmout_duty_cycle_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pwmio_pwmout_get_duty_cycle_obj,
              (mp_obj_t)&pwmio_pwmout_set_duty_cycle_obj,
              MP_ROM_NONE},
};

//|     frequency: int
//|     """32 bit value that dictates the PWM frequency in Hertz (cycles per
//|     second). Only writeable when constructed with ``variable_frequency=True``.
//|
//|     Depending on how PWM is implemented on a specific board, the internal value
//|     for the PWM's duty cycle may need to be recalculated when the frequency
//|     changes. In these cases, the duty cycle is automatically recalculated
//|     from the original duty cycle value. This should happen without any need
//|     to manually re-set the duty cycle."""
//|
STATIC mp_obj_t pwmio_pwmout_obj_get_frequency(mp_obj_t self_in) {
    pwmio_pwmout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_pwmio_pwmout_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(pwmio_pwmout_get_frequency_obj, pwmio_pwmout_obj_get_frequency);

STATIC mp_obj_t pwmio_pwmout_obj_set_frequency(mp_obj_t self_in, mp_obj_t frequency) {
    pwmio_pwmout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (!common_hal_pwmio_pwmout_get_variable_frequency(self)) {
        mp_raise_AttributeError(translate(
            "PWM frequency not writable when variable_frequency is False on "
            "construction."));
    }
    common_hal_pwmio_pwmout_set_frequency(self, mp_obj_get_int(frequency));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pwmio_pwmout_set_frequency_obj, pwmio_pwmout_obj_set_frequency);

const mp_obj_property_t pwmio_pwmout_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pwmio_pwmout_get_frequency_obj,
              (mp_obj_t)&pwmio_pwmout_set_frequency_obj,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t pwmio_pwmout_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pwmio_pwmout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&pwmio_pwmout___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_duty_cycle), MP_ROM_PTR(&pwmio_pwmout_duty_cycle_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&pwmio_pwmout_frequency_obj) },
    // TODO(tannewt): Add enabled to determine whether the signal is output
    // without giving up the resources. Useful for IR output.
};
STATIC MP_DEFINE_CONST_DICT(pwmio_pwmout_locals_dict, pwmio_pwmout_locals_dict_table);

const mp_obj_type_t pwmio_pwmout_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWMOut,
    .make_new = pwmio_pwmout_make_new,
    .locals_dict = (mp_obj_dict_t *)&pwmio_pwmout_locals_dict,
};
