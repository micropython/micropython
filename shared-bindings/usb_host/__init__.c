/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/usb_host/__init__.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/usb.h"

//| """USB Host
//|
//| The `usb_host` module allows you to manage USB host ports. To communicate
//| with devices use the `usb` module that is a subset of PyUSB's API.
//| """
//|
//| def set_user_keymap(keymap: ReadableBuffer, /) -> None:
//|     """Set the keymap used by a USB HID keyboard in kernel mode
//|
//|     The keymap consists of 256 or 384 1-byte entries that map from USB keycodes
//|     to ASCII codes. The first 128 entries are for unmodified keys,
//|     the next 128 entries are for shifted keys,and the next optional 128 entries are
//|     for altgr-modified keys.
//|
//|     The values must all be ASCII (32 through 126 inclusive); other values are not valid.
//|
//|     The values at index 0, 128, and 256 (if the keymap has 384 entries) must be
//|     0; other values are reserved for future expansion to indicate alternate
//|     keymap formats.
//|
//|     At other indices, the value 0 is used to indicate that the normal
//|     definition is still used. For instance, the entry for HID_KEY_ARROW_UP
//|     (0x52) is usually 0 so that the default behavior of sending an escape code
//|     is preserved.
//|
//|     This function is a CircuitPython extension not present in PyUSB
//|     """
//|
STATIC mp_obj_t usb_set_user_keymap(mp_obj_t buf_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    usb_keymap_set(bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(usb_set_user_keymap_obj, usb_set_user_keymap);

STATIC mp_map_elem_t usb_host_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_usb_host) },
    { MP_ROM_QSTR(MP_QSTR_Port),          MP_OBJ_FROM_PTR(&usb_host_port_type) },
    { MP_ROM_QSTR(MP_QSTR_set_user_keymap),          MP_OBJ_FROM_PTR(&usb_set_user_keymap_obj) },
};

STATIC MP_DEFINE_CONST_DICT(usb_host_module_globals, usb_host_module_globals_table);

const mp_obj_module_t usb_host_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_host_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_host, usb_host_module);
