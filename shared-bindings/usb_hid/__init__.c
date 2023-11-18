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

//| devices: Tuple[Device, ...]
//| """Tuple of all active HID device interfaces.
//| The default set of devices is ``Device.KEYBOARD, Device.MOUSE, Device.CONSUMER_CONTROL``,
//| On boards where `usb_hid` is disabled by default, `devices` is an empty tuple.
//|
//| If a boot device is enabled by `usb_hid.enable()`, *and* the host has requested a boot device,
//| the `devices` tuple is **replaced** when ``code.py`` starts with a single-element tuple
//| containing a `Device` that describes the boot device chosen (keyboard or mouse).
//| The request for a boot device overrides any other HID devices.
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
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_hid_disable_obj, usb_hid_disable);

//| def enable(devices: Optional[Sequence[Device]], boot_device: int = 0) -> None:
//|     """Specify which USB HID devices that will be available.
//|     Can be called in ``boot.py``, before USB is connected.
//|
//|     :param Sequence devices: `Device` objects.
//|       If `devices` is empty, HID is disabled. The order of the ``Devices``
//|       may matter to the host. For instance, for MacOS, put the mouse device
//|       before any Gamepad or Digitizer HID device or else it will not work.
//|     :param int boot_device: If non-zero, inform the host that support for a
//|       a boot HID device is available.
//|       If ``boot_device=1``, a boot keyboard is available.
//|       If ``boot_device=2``, a boot mouse is available. No other values are allowed.
//|       See below.
//|
//|     If you enable too many devices at once, you will run out of USB endpoints.
//|     The number of available endpoints varies by microcontroller.
//|     CircuitPython will go into safe mode after running ``boot.py`` to inform you if
//|     not enough endpoints are available.
//|
//|     **Boot Devices**
//|
//|     Boot devices implement a fixed, predefined report descriptor, defined in
//|     https://www.usb.org/sites/default/files/hid1_12.pdf, Appendix B. A USB host
//|     can request to use the boot device if the USB device says it is available.
//|     Usually only a BIOS or other kind of limited-functionality
//|     host needs boot keyboard support.
//|
//|     For example, to make a boot keyboard available, you can use this code::
//|
//|       usb_hid.enable((Device.KEYBOARD), boot_device=1)  # 1 for a keyboard
//|
//|     If the host requests the boot keyboard, the report descriptor provided by `Device.KEYBOARD`
//|     will be ignored, and the predefined report descriptor will be used.
//|     But if the host does not request the boot keyboard,
//|     the descriptor provided by `Device.KEYBOARD` will be used.
//|
//|     The HID boot device must usually be the first or only device presented by CircuitPython.
//|     The HID device will be USB interface number 0.
//|     To make sure it is the first device, disable other USB devices, including CDC and MSC (CIRCUITPY).
//|     If you specify a non-zero ``boot_device``, and it is not the first device, CircuitPython
//|     will enter safe mode to report this error.
//|     """
//|     ...
//|
STATIC mp_obj_t usb_hid_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_devices, ARG_boot_device };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_devices, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_boot_device, MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t devices = args[ARG_devices].u_obj;
    const mp_int_t len = mp_obj_get_int(mp_obj_len(devices));
    for (mp_int_t i = 0; i < len; i++) {
        mp_obj_t item = mp_obj_subscr(devices, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
        mp_arg_validate_type(item, &usb_hid_device_type, MP_QSTR___class__);
    }

    uint8_t boot_device =
        (uint8_t)mp_arg_validate_int_range(args[ARG_boot_device].u_int, 0, 2, MP_QSTR_boot_device);

    if (!common_hal_usb_hid_enable(devices, boot_device)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(usb_hid_enable_obj, 1, usb_hid_enable);

//| def get_boot_device() -> int:
//|     """
//|     :return: the boot device requested by the host, if any.
//|       Returns 0 if the host did not request a boot device, or if `usb_hid.enable()`
//|       was called with ``boot_device=0``, the default, which disables boot device support.
//|       If the host did request a boot device,
//|       returns the value of ``boot_device`` set in `usb_hid.enable()`:
//|       ``1`` for a boot keyboard, or ``2`` for boot mouse.
//|       However, the standard devices provided by CircuitPython, `Device.KEYBOARD` and `Device.MOUSE`,
//|       describe reports that match the boot device reports, so you don't need to check this
//|       if you are using those devices.
//|     :rtype int:
//|     """
//|
STATIC mp_obj_t usb_hid_get_boot_device(void) {
    return MP_OBJ_NEW_SMALL_INT(common_hal_usb_hid_get_boot_device());
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_hid_get_boot_device_obj, usb_hid_get_boot_device);

// usb_hid.devices is set once the usb devices are determined, after boot.py runs.
STATIC mp_map_elem_t usb_hid_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_usb_hid) },
    { MP_ROM_QSTR(MP_QSTR_Device),          MP_OBJ_FROM_PTR(&usb_hid_device_type) },
    { MP_ROM_QSTR(MP_QSTR_devices),         mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_disable),         MP_OBJ_FROM_PTR(&usb_hid_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),          MP_OBJ_FROM_PTR(&usb_hid_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_boot_device), MP_OBJ_FROM_PTR(&usb_hid_get_boot_device_obj) },
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

MP_REGISTER_MODULE(MP_QSTR_usb_hid, usb_hid_module);
