/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/usb_hid/__init__.h"
#include "shared-bindings/usb_hid/Device.h"

//| """USB Human Interface Device
//|
//| The `usb_hid` module allows you to output data as a HID device."""
//|

//| devices: Tuple[Device, ...]
//| """Tuple of all active HID device interfaces."""
//|

//| def configure_usb(devices: Optional[Sequence[Device, ...]]) -> None:
//|     """Configure the USB HID devices that will be available.
//|     Can be called in ``boot.py``, before USB is connected.
//|
//|     :param Sequence devices: `Device` objects.
//|       If `devices` is empty, HID is disabled. The order of the ``Devices``
//|       may matter to the host. For instance, for MacOS, put the mouse device
//|       before any Gamepad or Digitizer HID device or else it will not work.
//|     ...
//|
STATIC mp_obj_t usb_hid_configure_usb(mp_obj_t devices) {

    mp_obj_iter_buf_t iter_buf;

    const mp_int_t len = mp_obj_get_int(mp_obj_len(devices_seq));
    for (size_t i = 0; i < len; i++) {
        mp_obj_t item = mp_obj_subscr(devices_seq, mp_obj_new_small_int(i), MP_OBJ_SENTINEL);
        if (!MP_OBJ_IS_TYPE(item, &usb_hid_device_type)) {
            mp_raise_ValueError_varg(translate("non-Device in %q", MP_QSTR_devices));
        }
    }

    if (!common_hal_usb_hid_configure_usb(descriptors)) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_hid_configure_usb_obj, usb_hid_configure_usb);


STATIC const mp_rom_map_elem_t usb_hid_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),      MP_OBJ_NEW_QSTR(MP_QSTR_usb_hid) },
    { MP_ROM_QSTR(MP_QSTR_configure_usb), MP_OBJ_FROM_PTR(&usb_midi_configure_usb_obj) },
    { MP_ROM_QSTR(MP_QSTR_devices),       MP_ROM_PTR(&common_hal_usb_hid_devices) },
    { MP_ROM_QSTR(MP_QSTR_Device),        MP_ROM_PTR(&usb_hid_device_type) },
};

STATIC MP_DEFINE_CONST_DICT(usb_hid_module_globals, usb_hid_module_globals_table);

const mp_obj_module_t usb_hid_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_hid_module_globals,
};
