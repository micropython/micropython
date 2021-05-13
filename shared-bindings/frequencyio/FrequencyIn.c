/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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
#include "py/runtime0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/frequencyio/FrequencyIn.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class FrequencyIn:
//|     """Read a frequency signal
//|
//|     FrequencyIn is used to measure the frequency, in hertz, of a digital signal
//|     on an incoming pin. Accuracy has shown to be within 10%, if not better. It
//|     is recommended to utilize an average of multiple samples to smooth out readings.
//|
//|     Frequencies below 1KHz are not currently detectable.
//|
//|     FrequencyIn will not determine pulse width (use ``PulseIn``)."""
//|
//|     def __init__(self, pin: microcontroller.Pin, capture_period: int = 10) -> None:
//|         """Create a FrequencyIn object associated with the given pin.
//|
//|         :param ~microcontroller.Pin pin: Pin to read frequency from.
//|         :param int capture_period: Keyword argument to set the measurement period, in
//|                                    milliseconds. Default is 10ms; range is 1ms - 500ms.
//|
//|         Read the incoming frequency from a pin::
//|
//|           import frequencyio
//|           import board
//|
//|           frequency = frequencyio.FrequencyIn(board.D11)
//|
//|           # Loop while printing the detected frequency
//|           while True:
//|               print(frequency.value)
//|
//|               # Optional clear() will reset the value
//|               # to zero. Without this, if the incoming
//|               # signal stops, the last reading will remain
//|               # as the value.
//|               frequency.clear()"""
//|         ...
//|
STATIC mp_obj_t frequencyio_frequencyin_make_new(const mp_obj_type_t *type, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 1, 1, true);

    frequencyio_frequencyin_obj_t *self = m_new_obj(frequencyio_frequencyin_obj_t);
    self->base.type = &frequencyio_frequencyin_type;
    enum { ARG_pin, ARG_capture_period };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_capture_period, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 10} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj);

    const uint16_t capture_period = args[ARG_capture_period].u_int;

    common_hal_frequencyio_frequencyin_construct(self, pin, capture_period);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the FrequencyIn and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t frequencyio_frequencyin_deinit(mp_obj_t self_in) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_frequencyio_frequencyin_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(frequencyio_frequencyin_deinit_obj, frequencyio_frequencyin_deinit);

STATIC void check_for_deinit(frequencyio_frequencyin_obj_t *self) {
    if (common_hal_frequencyio_frequencyin_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> FrequencyIn:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t frequencyio_frequencyin_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_frequencyio_frequencyin_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(frequencyio_frequencyin___exit___obj, 4, 4, frequencyio_frequencyin_obj___exit__);

//|     def pause(self) -> None:
//|         """Pause frequency capture."""
//|         ...
//|
STATIC mp_obj_t frequencyio_frequencyin_obj_pause(mp_obj_t self_in) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_frequencyio_frequencyin_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(frequencyio_frequencyin_pause_obj, frequencyio_frequencyin_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes frequency capture."""
//|         ...
//|
STATIC mp_obj_t frequencyio_frequencyin_obj_resume(mp_obj_t self_in) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_frequencyio_frequencyin_resume(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(frequencyio_frequencyin_resume_obj, frequencyio_frequencyin_obj_resume);

//|     def clear(self) -> None:
//|         """Clears the last detected frequency capture value."""
//|         ...
//|

STATIC mp_obj_t frequencyio_frequencyin_obj_clear(mp_obj_t self_in) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_frequencyio_frequencyin_clear(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(frequencyio_frequencyin_clear_obj, frequencyio_frequencyin_obj_clear);

//|     capture_period: int
//|     """The capture measurement period. Lower incoming frequencies will be measured
//|     more accurately with longer capture periods. Higher frequencies are more
//|     accurate with shorter capture periods.
//|
//|     .. note:: When setting a new ``capture_period``, all previous capture information is
//|               cleared with a call to ``clear()``."""
//|
STATIC mp_obj_t frequencyio_frequencyin_obj_get_capture_period(mp_obj_t self_in) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return MP_OBJ_NEW_SMALL_INT(common_hal_frequencyio_frequencyin_get_capture_period(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(frequencyio_frequency_get_capture_period_obj, frequencyio_frequencyin_obj_get_capture_period);

STATIC mp_obj_t frequencyio_frequencyin_obj_set_capture_period(mp_obj_t self_in, mp_obj_t capture_period) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_frequencyio_frequencyin_set_capture_period(self, mp_obj_get_int(capture_period));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(frequencyio_frequency_set_capture_period_obj, frequencyio_frequencyin_obj_set_capture_period);

const mp_obj_property_t frequencyio_frequencyin_capture_period_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&frequencyio_frequency_get_capture_period_obj,
              (mp_obj_t)&frequencyio_frequency_set_capture_period_obj,
              MP_ROM_NONE},
};

//|     def __get__(self, index: int) -> int:
//|         """Returns the value of the last frequency captured."""
//|         ...
//|
STATIC mp_obj_t frequencyio_frequencyin_obj_get_value(mp_obj_t self_in) {
    frequencyio_frequencyin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    // return MP_OBJ_NEW_SMALL_INT(common_hal_frequencyio_frequencyin_get_item(self));
    return mp_obj_new_int_from_float(common_hal_frequencyio_frequencyin_get_item(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(frequencyio_frequencyin_get_value_obj, frequencyio_frequencyin_obj_get_value);

const mp_obj_property_t frequencyio_frequencyin_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&frequencyio_frequencyin_get_value_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t frequencyio_frequencyin_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&frequencyio_frequencyin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&frequencyio_frequencyin___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&frequencyio_frequencyin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&frequencyio_frequencyin_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&frequencyio_frequencyin_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&frequencyio_frequencyin_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture_period), MP_ROM_PTR(&frequencyio_frequencyin_capture_period_obj) },
};
STATIC MP_DEFINE_CONST_DICT(frequencyio_frequencyin_locals_dict, frequencyio_frequencyin_locals_dict_table);

const mp_obj_type_t frequencyio_frequencyin_type = {
    { &mp_type_type },
    .name = MP_QSTR_frequencyin,
    .make_new = frequencyio_frequencyin_make_new,
    .locals_dict = (mp_obj_dict_t *)&frequencyio_frequencyin_locals_dict,
};
