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
#include "GamePad.h"
#include "GamePadShift.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/util.h"


// Helper for validating digitalio.DigitalInOut arguments
digitalio_digitalinout_obj_t *pin_io(mp_obj_t obj) {
    if (!MP_OBJ_IS_TYPE(obj, &digitalio_digitalinout_type)) {
        mp_raise_TypeError(translate("argument num/types mismatch"));
    }
    digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(obj);
    raise_error_if_deinited(
        common_hal_digitalio_digitalinout_deinited(pin));
    return pin;
}


//| :mod:`gamepad` --- Button handling
//| ==================================
//|
//| .. module:: gamepad
//|   :synopsis: Button handling
//|   :platform: SAMD21
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     GamePad
//|
STATIC const mp_rom_map_elem_t gamepad_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gamepad) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GamePad),  MP_ROM_PTR(&gamepad_type)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_GamePadShift),  MP_ROM_PTR(&gamepadshift_type)},
};
STATIC MP_DEFINE_CONST_DICT(gamepad_module_globals,
        gamepad_module_globals_table);

const mp_obj_module_t gamepad_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&gamepad_module_globals,
};
