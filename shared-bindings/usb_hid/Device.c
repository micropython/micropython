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

//| class Device:
//|     """HID Device
//|
//|     Usage::
//|
//|        import usb_hid
//|
//|        mouse = usb_hid.devices[0]
//|
//|        mouse.send_report()"""
//|

//|     def __init__(self) -> None:
//|         """Not currently dynamically supported."""
//|         ...
//|
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
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
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
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
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
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t usb_hid_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_report),          MP_ROM_PTR(&usb_hid_device_send_report_obj) },
    { MP_ROM_QSTR(MP_QSTR_last_received_report), MP_ROM_PTR(&usb_hid_device_last_received_report_obj) },
    { MP_ROM_QSTR(MP_QSTR_usage_page),           MP_ROM_PTR(&usb_hid_device_usage_page_obj)},
    { MP_ROM_QSTR(MP_QSTR_usage),                MP_ROM_PTR(&usb_hid_device_usage_obj)},
};

STATIC MP_DEFINE_CONST_DICT(usb_hid_device_locals_dict, usb_hid_device_locals_dict_table);

const mp_obj_type_t usb_hid_device_type = {
    { &mp_type_type },
    .name = MP_QSTR_Device,
    .locals_dict = (mp_obj_t)&usb_hid_device_locals_dict,
};
