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
#include "shared-bindings/util.h"
#include "supervisor/shared/translate/translate.h"

// RGB LED timing information:

// From the WS2811 datasheet: high speed mode
// - T0H 0 code,high voltage time 0.25 us +-150ns
// - T1H 1 code,high voltage time 0.6  us +-150ns
// - T0L 0 code,low voltage time 1.0 us +-150ns
// - T1L 1 code,low voltage time 0.65 us +-150ns
// - RES low voltage time Above 50us

// From the SK6812 datasheet:
// - T0H 0 code, high level time 0.3us +-0.15us
// - T1H 1 code, high level time 0.6us +-0.15us
// - T0L 0 code, low level time 0.9us +-0.15us
// - T1L 1 code, low level time 0.6us +-0.15us
// - Trst Reset code，low level time 80us

// From the WS2812 datasheet:
// - T0H 0 code, high voltage time 0.35us +-150ns
// - T1H 1 code, high voltage time 0.7us +-150ns
// - T0L 0 code, low voltage time 0.8us +-150ns
// - T1L 1 code, low voltage time 0.6us +-150ns
// - RES low voltage time Above 50us

// From the WS28212B datasheet:
// - T0H 0 code, high voltage time 0.4us +-150ns
// - T1H 1 code, high voltage time 0.8us +-150ns
// - T0L 0 code, low voltage time 0.85us +-150ns
// - T1L 1 code, low voltage time 0.45us +-150ns
// - RES low voltage time Above 50us

// The timings used in various ports do not always follow the guidelines above.
// In general, a zero bit is about 300ns high, 900ns low.
// A one bit is about 700ns high, 500ns low.
// But the ports vary based on implementation considerations; the proof is in the testing.
// https://adafru.it/5225 is more sensitive to timing and should be included in testing.

STATIC void check_for_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        raise_deinited_error();
    }
}

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
//|   neopixel_write.neopixel_write(pin, pixel_off)
//|
//| .. note::
//|
//|     This module is typically not used by user level code.
//|
//|     For more information on actually using NeoPixels, refer to the `CircuitPython
//|     Essentials Learn guide <https://learn.adafruit.com/circuitpython-essentials/circuitpython-neopixel>`_
//|
//|     For a much more thorough guide about using NeoPixels, refer to the `Adafruit NeoPixel Überguide
//|     <https://learn.adafruit.com/adafruit-neopixel-uberguide>`_.
//|
//| """
//|
//| def neopixel_write(digitalinout: digitalio.DigitalInOut, buf: ReadableBuffer) -> None:
//|     """Write buf out on the given DigitalInOut.
//|
//|     :param ~digitalio.DigitalInOut digitalinout: the DigitalInOut to output with
//|     :param ~circuitpython_typing.ReadableBuffer buf: The bytes to clock out. No assumption is made about color order
//|     """
//|     ...
//|
STATIC mp_obj_t neopixel_write_neopixel_write_(mp_obj_t digitalinout_obj, mp_obj_t buf) {
    const digitalio_digitalinout_obj_t *digitalinout =
        mp_arg_validate_type(digitalinout_obj, &digitalio_digitalinout_type, MP_QSTR_digitalinout);

    // Check to see if the NeoPixel has been deinited before writing to it.
    check_for_deinit(digitalinout_obj);

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

MP_REGISTER_MODULE(MP_QSTR_neopixel_write, neopixel_write_module);
