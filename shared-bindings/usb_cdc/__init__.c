/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbertfor Adafruit Industries
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

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/usb_cdc/__init__.h"
#include "shared-bindings/usb_cdc/Serial.h"

#include "py/runtime.h"

//| """USB CDC Serial streams
//|
//| The `usb_cdc` module allows access to USB CDC (serial) communications.
//|
//| On Windows, each `Serial` is visible as a separate COM port. The ports will often
//| be assigned consecutively, REPL first, but this is not always true.
//|
//| On Linux, the ports are typically ``/dev/ttyACM0`` and ``/dev/ttyACM1``. The REPL
//| is usually first.
//|
//| On MacOS, the ports are typically ``/dev/cu.usbmodem<something>``. The something
//| varies based on the USB bus and port used. The REPL is usually first.
//| """
//|
//| repl: Optional[Serial]
//| """The `Serial` object that can be used to communicate over the REPL serial
//| channel. ``None`` if disabled.
//|
//| Note that`sys.stdin` and `sys.stdout` are also connected to the REPL, though
//| they are text-based streams, and the `repl` object is a binary stream."""
//|
//| data: Optional[Serial]
//| """A `Serial` object that can be used to send and receive binary data to and from
//| the host.
//| Note that `data` is *disabled* by default."""




//| def enable_repl(enabled:bool) -> None:
//|     """Enable or disable the `repl` USB serial connection. The REPL
//|     is enabled by default.
//|     Can be changed in ``boot.py``, before USB is connected."""
//|     ...
//|
STATIC mp_obj_t usb_cdc_enable_repl(mp_obj_t enabled) {
    if (!common_hal_usb_cdc_enable_repl(mp_obj_is_true(enabled))) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_enable_repl_obj, usb_cdc_enable_repl);

//| def enable_data(enabled: bool) -> None:
//|     """Enable or disable the `data` USB serial connection;n
//|     *disabled* by default.
//|     Can be changed in ``boot.py``, before USB is connected."""
//|     ...
//|
STATIC mp_obj_t usb_cdc_enable_data(mp_obj_t enabled) {
    if (!common_hal_usb_cdc_enable_data(mp_obj_is_true(enabled))) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_enable_data_obj, usb_cdc_enable_data);

// The usb_cdc module dict is mutable so that .repl and .data may
// be set to a Serial or to None depending on whether they are enabled or not.
static mp_map_elem_t usb_cdc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_usb_cdc) },
    { MP_ROM_QSTR(MP_QSTR_Serial),      MP_OBJ_FROM_PTR(&usb_cdc_serial_type) },
    { MP_ROM_QSTR(MP_QSTR_repl),        mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_data),        mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_enable_repl), MP_OBJ_FROM_PTR(&usb_cdc_enable_repl_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_data), MP_OBJ_FROM_PTR(&usb_cdc_enable_data_obj) },
};

static MP_DEFINE_MUTABLE_DICT(usb_cdc_module_globals, usb_cdc_module_globals_table);

const mp_obj_module_t usb_cdc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_cdc_module_globals,
};
