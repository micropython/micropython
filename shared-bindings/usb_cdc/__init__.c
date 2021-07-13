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
//| be assigned consecutively, `console` first, but this is not always true.
//|
//| On Linux, the ports are typically ``/dev/ttyACM0`` and ``/dev/ttyACM1``.
//| The `console` port will usually be first.
//|
//| On MacOS, the ports are typically ``/dev/cu.usbmodem<something>``. The something
//| varies based on the USB bus and port used. The `console` port will usually be first.
//| """
//|
//| console: Optional[Serial]
//| """The `console` `Serial` object is used for the REPL, and for `sys.stdin` and `sys.stdout`.
//|    `console` is ``None`` if disabled.
//|
//| However, note that `sys.stdin` and `sys.stdout` are text-based streams,
//| and the `console` object is a binary stream.
//| You do not normally need to write to `console` unless you want to write binary data.
//| """
//|
//| data: Optional[Serial]
//| """A `Serial` object that can be used to send and receive binary data to and from
//| the host.
//| Note that `data` is *disabled* by default. ``data`` is ``None`` if disabled."""

//| def disable() -> None:
//|     """Do not present any USB CDC device to the host.
//|     Can be called in ``boot.py``, before USB is connected.
//|     Equivalent to ``usb_cdc.enable(console=False, data=False)``."""
//|     ...
//|
STATIC mp_obj_t usb_cdc_disable(void) {
    if (!common_hal_usb_cdc_disable()) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_cdc_disable_obj, usb_cdc_disable);

//| def enable(*, console: bool = True, data: bool = False) -> None:
//|     """Enable or disable each CDC device. Can be called in ``boot.py``, before USB is connected.
//|
//|     :param console bool: Enable or disable the `console` USB serial device.
//|       True to enable; False to disable. Enabled by default.
//|     :param data bool: Enable or disable the `data` USB serial device.
//|       True to enable; False to disable. *Disabled* by default.
//|
//|     If you enable too many devices at once, you will run out of USB endpoints.
//|     The number of available endpoints varies by microcontroller.
//|     CircuitPython will go into safe mode after running boot.py to inform you if
//|     not enough endpoints are available.
//|     """
//|     ...
//|
STATIC mp_obj_t usb_cdc_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_console, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_console, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true } },
        { MP_QSTR_data, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!common_hal_usb_cdc_enable(args[ARG_console].u_bool, args[ARG_data].u_bool)) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(usb_cdc_enable_obj, 0, usb_cdc_enable);

// The usb_cdc module dict is mutable so that .console and .data may
// be set to a Serial or to None depending on whether they are enabled or not.
static mp_map_elem_t usb_cdc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_cdc) },
    { MP_ROM_QSTR(MP_QSTR_Serial),   MP_OBJ_FROM_PTR(&usb_cdc_serial_type) },
    { MP_ROM_QSTR(MP_QSTR_console),  mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_data),     mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_disable),  MP_OBJ_FROM_PTR(&usb_cdc_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),   MP_OBJ_FROM_PTR(&usb_cdc_enable_obj) },
};

static MP_DEFINE_MUTABLE_DICT(usb_cdc_module_globals, usb_cdc_module_globals_table);

const mp_obj_module_t usb_cdc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_cdc_module_globals,
};

static void set_module_dict_entry(mp_obj_t key_qstr, mp_obj_t serial_obj) {
    mp_map_elem_t *elem = mp_map_lookup(&usb_cdc_module_globals.map, key_qstr, MP_MAP_LOOKUP);
    if (elem) {
        elem->value = serial_obj;
    }
}

void usb_cdc_set_console(mp_obj_t serial_obj) {
    set_module_dict_entry(MP_ROM_QSTR(MP_QSTR_console), serial_obj);
}

void usb_cdc_set_data(mp_obj_t serial_obj) {
    set_module_dict_entry(MP_ROM_QSTR(MP_QSTR_data), serial_obj);
}
