/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/objproperty.h"
#include "shared-bindings/usb_hid/Device.h"
#include "py/runtime.h"

//| class Device:
//|     """HID Device specification"""
//|
//|     def __init__(self, *, descriptor: ReadableBuffer, usage_page: int, usage: int, in_report_length: int, out_report_length: int = 0, report_id_index: Optional[int]) -> None:
//|         """Create a description of a USB HID device. The actual device is created when you
//|         pass a `Device` to `usb_hid.enable()`.
//|
//|         :param ReadableBuffer report_descriptor: The USB HID Report descriptor bytes. The descriptor is not
//|           not verified for correctness; it is up to you to make sure it is not malformed.
//|         :param int usage_page: The Usage Page value from the descriptor. Must match what is in the descriptor.
//|         :param int usage: The Usage value from the descriptor. Must match what is in the descriptor.
//|         :param int in_report_length: Size in bytes of the HID report sent to the host.
//|           "In" is with respect to the host.
//|         :param int out_report_length: Size in bytes of the HID report received from the host.
//|           "Out" is with respect to the host. If no reports are expected, use 0.
//|         :param int report_id_index: position of byte in descriptor that contains the Report ID.
//|           A Report ID will be assigned when the device is created. If there is no
//|           Report ID, use ``None``.
//|         """
//|         ...
//|
//|     KEYBOARD: Device
//|     """Standard keyboard device supporting keycodes 0x00-0xDD, modifiers 0xE-0xE7, and five LED indicators."""
//|
//|     MOUSE: Device
//|     """Standard mouse device supporting five mouse buttons, X and Y relative movements from -127 to 127
//|     in each report, and a relative mouse wheel change from -127 to 127 in each report."""
//|
//|     CONSUMER_CONTROL: Device
//|     """Consumer Control device supporting sent values from 1-652, with no rollover."""
//|

STATIC mp_obj_t usb_hid_device_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    usb_hid_device_obj_t *self = m_new_obj(usb_hid_device_obj_t);
    self->base.type = &usb_hid_device_type;
    enum { ARG_report_descriptor, ARG_usage_page, ARG_usage, ARG_in_report_length, ARG_out_report_length, ARG_report_id_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_report_descriptor, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_usage_page, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_usage, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_in_report_length, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_out_report_length, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_report_id_index, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t descriptor_bufinfo;
    mp_get_buffer_raise(args[ARG_report_descriptor].u_obj, &descriptor_bufinfo, MP_BUFFER_READ);
    mp_obj_t descriptor = mp_obj_new_bytearray(descriptor_bufinfo.len, descriptor_bufinfo.buf);

    const mp_int_t usage_page_arg = args[ARG_usage_page].u_int;
    if (usage_page_arg <= 0 || usage_page_arg > 255) {
        mp_raise_ValueError_varg(translate("%q must be 1-255"), MP_QSTR_usage_page);
    }
    const uint8_t usage_page = usage_page_arg;

    const mp_int_t usage_arg = args[ARG_usage].u_int;
    if (usage_arg <= 0 || usage_arg > 255) {
        mp_raise_ValueError_varg(translate("%q must be 1-255"), MP_QSTR_usage);
    }
    const uint8_t usage = usage_arg;

    const mp_int_t in_report_length_arg = args[ARG_in_report_length].u_int;
    if (in_report_length_arg <= 0 || in_report_length_arg > 255) {
        mp_raise_ValueError_varg(translate("%q must be 1-255"), MP_QSTR_in_report_length);
    }
    const uint8_t in_report_length = in_report_length_arg;

    const mp_int_t out_report_length_arg = args[ARG_out_report_length].u_int;
    if (out_report_length_arg < 0 || out_report_length_arg > 255) {
        mp_raise_ValueError_varg(translate("%q must be 0-255"), MP_QSTR_out_report_length);
    }
    const uint8_t out_report_length = out_report_length_arg;

    const mp_obj_t report_id_index_arg = args[ARG_report_id_index].u_obj;
    uint8_t report_id_index = 0;
    if (report_id_index_arg != mp_const_none) {
        const mp_int_t report_id_index_int = mp_obj_int_get_checked(report_id_index_arg);
        if (report_id_index_int <= 0 || (uint32_t)report_id_index_int >= descriptor_bufinfo.len) {
            mp_raise_ValueError_varg(translate("%q must be None or between 1 and len(report_descriptor)-1"),
                MP_QSTR_report_id_index);
        }
        report_id_index = report_id_index_int;
    }

    common_hal_usb_hid_device_construct(
        self, descriptor, usage_page, usage, in_report_length, out_report_length, report_id_index);
    return (mp_obj_t)self;
}


//|     def send_report(self, buf: ReadableBuffer) -> None:
//|         """Send a HID report."""
//|         ...
//|
STATIC mp_obj_t usb_hid_device_send_report(mp_obj_t self_in, mp_obj_t buffer) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);

    common_hal_usb_hid_device_send_report(self, ((uint8_t *)bufinfo.buf), bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(usb_hid_device_send_report_obj, usb_hid_device_send_report);

//|     last_received_report: bytes
//|     """The HID OUT report as a `bytes`. (read-only). `None` if nothing received."""
//|
STATIC mp_obj_t usb_hid_device_obj_get_last_received_report(mp_obj_t self_in) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->out_report_buffer == 0) {
        return mp_const_none;
    }
    return mp_obj_new_bytes(self->out_report_buffer, self->out_report_length);
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_hid_device_get_last_received_report_obj, usb_hid_device_obj_get_last_received_report);

const mp_obj_property_t usb_hid_device_last_received_report_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_hid_device_get_last_received_report_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     usage_page: int
//|     """The usage page of the device as an `int`. Can be thought of a category. (read-only)"""
//|
STATIC mp_obj_t usb_hid_device_obj_get_usage_page(mp_obj_t self_in) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_usb_hid_device_get_usage_page(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_hid_device_get_usage_page_obj, usb_hid_device_obj_get_usage_page);

const mp_obj_property_t usb_hid_device_usage_page_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_hid_device_get_usage_page_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     usage: int
//|     """The functionality of the device as an int. (read-only)
//|
//|     For example, Keyboard is 0x06 within the generic desktop usage page 0x01.
//|     Mouse is 0x02 within the same usage page."""
//|
STATIC mp_obj_t usb_hid_device_obj_get_usage(mp_obj_t self_in) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_usb_hid_device_get_usage(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_hid_device_get_usage_obj,
    usb_hid_device_obj_get_usage);

const mp_obj_property_t usb_hid_device_usage_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_hid_device_get_usage_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t usb_hid_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_report),          MP_ROM_PTR(&usb_hid_device_send_report_obj) },
    { MP_ROM_QSTR(MP_QSTR_last_received_report), MP_ROM_PTR(&usb_hid_device_last_received_report_obj) },
    { MP_ROM_QSTR(MP_QSTR_usage_page),           MP_ROM_PTR(&usb_hid_device_usage_page_obj) },
    { MP_ROM_QSTR(MP_QSTR_usage),                MP_ROM_PTR(&usb_hid_device_usage_obj) },
    { MP_ROM_QSTR(MP_QSTR_KEYBOARD),             MP_ROM_PTR(&usb_hid_device_keyboard_obj) },
    { MP_ROM_QSTR(MP_QSTR_MOUSE),                MP_ROM_PTR(&usb_hid_device_mouse_obj) },
    { MP_ROM_QSTR(MP_QSTR_CONSUMER_CONTROL),     MP_ROM_PTR(&usb_hid_device_consumer_control_obj) },
};

STATIC MP_DEFINE_CONST_DICT(usb_hid_device_locals_dict, usb_hid_device_locals_dict_table);

const mp_obj_type_t usb_hid_device_type = {
    { &mp_type_type },
    .name = MP_QSTR_Device,
    .make_new = usb_hid_device_make_new,
    .locals_dict = (mp_obj_t)&usb_hid_device_locals_dict,
};
