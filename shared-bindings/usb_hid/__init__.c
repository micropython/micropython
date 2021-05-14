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
//| """Tuple of all active HID device interfaces.
//| The default set of devices is ``Device.KEYBOARD, Device.MOUSE, Device.CONSUMER_CONTROL``,
//| On boards where `usb_hid` is disabled by default, `devices` is an empty tuple.
//| """
//|

//| def disable() -> None:
//|     """Do not present any USB HID devices to the host computer.
//|     Can be called in ``boot.py``, before USB is connected.
//|     The HID composite device is normally enabled by default,
//|     but on some boards with limited endpoints, including STM32F4,
//|     it is disabled by default. You must turn off another USB device such
//|     as `usb_cdc` or `storage` to free up endpoints for use by `usb_hid`.
//|     """
//|
STATIC mp_obj_t usb_hid_disable(void) {
    if (!common_hal_usb_hid_disable()) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_hid_disable_obj, usb_hid_disable);

//| def enable(devices: Optional[Sequence[Device]]) -> None:
//|     """Specify which USB HID devices that will be available.
//|     Can be called in ``boot.py``, before USB is connected.
//|
//|     :param Sequence devices: `Device` objects.
//|       If `devices` is empty, HID is disabled. The order of the ``Devices``
//|       may matter to the host. For instance, for MacOS, put the mouse device
//|       before any Gamepad or Digitizer HID device or else it will not work.
//|
//|     If you enable too many devices at once, you will run out of USB endpoints.
//|     The number of available endpoints varies by microcontroller.
//|     CircuitPython will go into safe mode after running boot.py to inform you if
//|     not enough endpoints are available.
//|     """
//|     ...
//|
STATIC mp_obj_t usb_hid_enable(mp_obj_t devices) {
    const mp_int_t len = mp_obj_get_int(mp_obj_len(devices));
    for (mp_int_t i = 0; i < len; i++) {
        mp_obj_t item = mp_obj_subscr(devices, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
        if (!mp_obj_is_type(item, &usb_hid_device_type)) {
            mp_raise_ValueError_varg(translate("non-Device in %q"), MP_QSTR_devices);
        }
    }

    if (!common_hal_usb_hid_enable(devices)) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_hid_enable_obj, usb_hid_enable);

// usb_hid.devices is set once the usb devices are determined, after boot.py runs.
STATIC mp_map_elem_t usb_hid_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_usb_hid) },
    { MP_ROM_QSTR(MP_QSTR_Device),   MP_OBJ_FROM_PTR(&usb_hid_device_type) },
    { MP_ROM_QSTR(MP_QSTR_devices),  mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_disable),  MP_OBJ_FROM_PTR(&usb_hid_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),   MP_OBJ_FROM_PTR(&usb_hid_enable_obj) },
};

STATIC MP_DEFINE_MUTABLE_DICT(usb_hid_module_globals, usb_hid_module_globals_table);

const mp_obj_module_t usb_hid_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_hid_module_globals,
};

void usb_hid_set_devices(mp_obj_t devices) {
    mp_map_elem_t *elem =
        mp_map_lookup(&usb_hid_module_globals.map, MP_ROM_QSTR(MP_QSTR_devices), MP_MAP_LOOKUP);
    if (elem) {
        elem->value = devices;
    }
}
