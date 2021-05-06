/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Radomir Dopieralski for Adafruit Industries
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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/gc.h"
#include "py/mpstate.h"
#include "shared-bindings/gamepadshift/GamePadShift.h"
#include "shared-bindings/gamepadshift/__init__.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/tick.h"

//| class GamePadShift:
//|     """Scan buttons for presses through a shift register"""
//|
//|     def __init__(self, clock: digitalio.DigitalInOut, data: digitalio.DigitalInOut, latch: digitalio.DigitalInOut) -> None:
//|         """Initializes button scanning routines.
//|
//|         The ``clock``, ``data`` and ``latch`` parameters are ``DigitalInOut``
//|         objects connected to the shift register controlling the buttons.
//|
//|         The button presses are accumulated, until the ``get_pressed`` method
//|         is called, at which point the button state is cleared, and the new
//|         button presses start to be recorded.
//|
//|         Only one gamepad (`gamepad.GamePad` or `gamepadshift.GamePadShift`)
//|         may be used at a time."""
//|         ...
//|
STATIC mp_obj_t gamepadshift_make_new(const mp_obj_type_t *type, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_clock, ARG_data, ARG_latch };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ},
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_latch, MP_ARG_REQUIRED | MP_ARG_OBJ},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
        allowed_args, args);

    digitalio_digitalinout_obj_t *clock_pin = assert_digitalinout(args[ARG_clock].u_obj);
    digitalio_digitalinout_obj_t *data_pin = assert_digitalinout(args[ARG_data].u_obj);
    digitalio_digitalinout_obj_t *latch_pin = assert_digitalinout(args[ARG_latch].u_obj);

    gamepadshift_obj_t *gamepad_singleton = MP_STATE_VM(gamepad_singleton);
    if (!gamepad_singleton ||
        !mp_obj_is_type(MP_OBJ_FROM_PTR(gamepad_singleton),
            &gamepadshift_type)) {
        gamepad_singleton = m_new_ll_obj(gamepadshift_obj_t);
        gamepad_singleton->base.type = &gamepadshift_type;
        if (!MP_STATE_VM(gamepad_singleton)) {
            supervisor_enable_tick();
        }
        MP_STATE_VM(gamepad_singleton) = gamepad_singleton;
    }
    common_hal_gamepadshift_gamepadshift_init(gamepad_singleton, clock_pin, data_pin, latch_pin);
    return MP_OBJ_FROM_PTR(gamepad_singleton);
}

//|     def get_pressed(self) -> int:
//|         """Get the status of buttons pressed since the last call and clear it.
//|
//|         Returns an 8-bit number, with bits that correspond to buttons,
//|         which have been pressed (or held down) since the last call to this
//|         function set to 1, and the remaining bits set to 0. Then it clears
//|         the button state, so that new button presses (or buttons that are
//|         held down) can be recorded for the next call."""
//|         ...
//|
STATIC mp_obj_t gamepadshift_get_pressed(mp_obj_t self_in) {
    gamepadshift_obj_t *gamepad_singleton = MP_STATE_VM(gamepad_singleton);
    mp_obj_t pressed = MP_OBJ_NEW_SMALL_INT(gamepad_singleton->pressed);
    gamepad_singleton->pressed = gamepad_singleton->last;
    return pressed;
}
MP_DEFINE_CONST_FUN_OBJ_1(gamepadshift_get_pressed_obj, gamepadshift_get_pressed);

//|     def deinit(self) -> None:
//|         """Disable button scanning."""
//|         ...
//|
STATIC mp_obj_t gamepadshift_deinit(mp_obj_t self_in) {
    common_hal_gamepadshift_gamepadshift_deinit(self_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gamepadshift_deinit_obj, gamepadshift_deinit);


STATIC const mp_rom_map_elem_t gamepadshift_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_pressed),  MP_ROM_PTR(&gamepadshift_get_pressed_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),  MP_ROM_PTR(&gamepadshift_deinit_obj)},
};
STATIC MP_DEFINE_CONST_DICT(gamepadshift_locals_dict, gamepadshift_locals_dict_table);
const mp_obj_type_t gamepadshift_type = {
    { &mp_type_type },
    .name = MP_QSTR_GamePadShift,
    .make_new = gamepadshift_make_new,
    .locals_dict = (mp_obj_dict_t *)&gamepadshift_locals_dict,
};
