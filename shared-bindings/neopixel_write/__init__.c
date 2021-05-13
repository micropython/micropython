/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/neopixel_write/__init__.h"

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "supervisor/shared/translate.h"

//| """Low-level neopixel implementation
//|
//| The `neopixel_write` module contains a helper method to write out bytes in
//| the 800khz neopixel protocol.
//|
//| For example, to turn off a single neopixel (like the status pixel on Express
//| boards.)
//|
//| .. code-block:: python
//|
//|   import board
//|   import neopixel_write
//|   import digitalio
//|
//|   pin = digitalio.DigitalInOut(board.NEOPIXEL)
//|   pin.direction = digitalio.Direction.OUTPUT
//|   pixel_off = bytearray([0, 0, 0])
//|   neopixel_write.neopixel_write(pin, pixel_off)"""
//|
//| def neopixel_write(digitalinout: digitalio.DigitalInOut, buf: ReadableBuffer) -> None:
//|     """Write buf out on the given DigitalInOut.
//|
//|     :param ~digitalio.DigitalInOut digitalinout: the DigitalInOut to output with
//|     :param ~_typing.ReadableBuffer buf: The bytes to clock out. No assumption is made about color order"""
//|     ...
STATIC mp_obj_t neopixel_write_neopixel_write_(mp_obj_t digitalinout_obj, mp_obj_t buf) {
    if (!mp_obj_is_type(digitalinout_obj, &digitalio_digitalinout_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), digitalio_digitalinout_type.name);
    }
    // Convert parameters into expected types.
    const digitalio_digitalinout_obj_t *digitalinout = MP_OBJ_TO_PTR(digitalinout_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    // Call platform's neopixel write function with provided buffer and options.
    common_hal_neopixel_write(digitalinout, (uint8_t *)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(neopixel_write_neopixel_write_obj, neopixel_write_neopixel_write_);

STATIC const mp_rom_map_elem_t neopixel_write_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_neopixel_write) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_neopixel_write), (mp_obj_t)&neopixel_write_neopixel_write_obj },
};

STATIC MP_DEFINE_CONST_DICT(neopixel_write_module_globals, neopixel_write_module_globals_table);

const mp_obj_module_t neopixel_write_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&neopixel_write_module_globals,
};
