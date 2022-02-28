/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class PulseOut:
//|     """Pulse PWM "carrier" output on and off. This is commonly used in infrared remotes. The
//|        pulsed signal consists of timed on and off periods. Unlike PWM, there is no set duration
//|        for on and off pairs."""
//|
//|     def __init__(self, pin: microcontroller.Pin, *, frequency: int = 38000, duty_cycle: int = 1 << 15) -> None:
//|         """Create a PulseOut object associated with the given pin.
//|
//|         :param ~microcontroller.Pin pin: Signal output pin
//|         :param int frequency: Carrier signal frequency in Hertz
//|         :param int duty_cycle: 16-bit duty cycle of carrier frequency (0 - 65536)
//|
//|         For backwards compatibility, ``pin`` may be a PWMOut object used as the carrier. This
//|         compatibility will be removed in CircuitPython 8.0.0.
//|
//|         Send a short series of pulses::
//|
//|           import array
//|           import pulseio
//|           import pwmio
//|           import board
//|
//|           # 50% duty cycle at 38kHz.
//|           pulse = pulseio.PulseOut(board.LED, frequency=38000, duty_cycle=32768)
//|           #                             on   off     on    off    on
//|           pulses = array.array('H', [65000, 1000, 65000, 65000, 1000])
//|           pulse.send(pulses)
//|
//|           # Modify the array of pulses.
//|           pulses[0] = 200
//|           pulse.send(pulses)"""
//|         ...
//|
STATIC mp_obj_t pulseio_pulseout_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_frequency, ARG_duty_cycle};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_frequency, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 38000} },
        { MP_QSTR_duty_cycle, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1 << 15} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin = args[ARG_pin].u_obj;
    mp_int_t frequency = args[ARG_frequency].u_int;
    mp_int_t duty_cycle = args[ARG_duty_cycle].u_int;
    if (mp_obj_is_type(args[ARG_pin].u_obj, &pwmio_pwmout_type)) {
        pwmio_pwmout_obj_t *pwmout = args[ARG_pin].u_obj;
        duty_cycle = common_hal_pwmio_pwmout_get_duty_cycle(pwmout);
        frequency = common_hal_pwmio_pwmout_get_frequency(pwmout);
        pin = common_hal_pwmio_pwmout_get_pin(pwmout);
        // Deinit the pin so we can use it.
        common_hal_pwmio_pwmout_deinit(pwmout);
    }
    validate_obj_is_free_pin(MP_OBJ_FROM_PTR(pin));
    pulseio_pulseout_obj_t *self = m_new_obj(pulseio_pulseout_obj_t);
    self->base.type = &pulseio_pulseout_type;
    common_hal_pulseio_pulseout_construct(self, pin, frequency, duty_cycle);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the PulseOut and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t pulseio_pulseout_deinit(mp_obj_t self_in) {
    pulseio_pulseout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_pulseio_pulseout_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pulseio_pulseout_deinit_obj, pulseio_pulseout_deinit);

//|     def __enter__(self) -> PulseOut:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t pulseio_pulseout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_pulseio_pulseout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pulseio_pulseout___exit___obj, 4, 4, pulseio_pulseout_obj___exit__);

//|     def send(self, pulses: ReadableBuffer) -> None:
//|         """Pulse alternating on and off durations in microseconds starting with on.
//|         ``pulses`` must be an `array.array` with data type 'H' for unsigned
//|         halfword (two bytes).
//|
//|         This method waits until the whole array of pulses has been sent and
//|         ensures the signal is off afterwards.
//|
//|         :param array.array pulses: pulse durations in microseconds"""
//|         ...
//|
STATIC mp_obj_t pulseio_pulseout_obj_send(mp_obj_t self_in, mp_obj_t pulses) {
    pulseio_pulseout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_pulseio_pulseout_deinited(self)) {
        raise_deinited_error();
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(pulses, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.typecode != 'H') {
        mp_raise_TypeError(translate("Array must contain halfwords (type 'H')"));
    }
    common_hal_pulseio_pulseout_send(self, (uint16_t *)bufinfo.buf, bufinfo.len / 2);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pulseio_pulseout_send_obj, pulseio_pulseout_obj_send);

STATIC const mp_rom_map_elem_t pulseio_pulseout_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pulseio_pulseout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&pulseio_pulseout___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pulseio_pulseout_send_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pulseio_pulseout_locals_dict, pulseio_pulseout_locals_dict_table);

const mp_obj_type_t pulseio_pulseout_type = {
    { &mp_type_type },
    .name = MP_QSTR_PulseOut,
    .make_new = pulseio_pulseout_make_new,
    .locals_dict = (mp_obj_dict_t *)&pulseio_pulseout_locals_dict,
};
