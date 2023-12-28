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
//|     def __init__(
//|         self,
//|         *,
//|         report_descriptor: ReadableBuffer,
//|         usage_page: int,
//|         usage: int,
//|         report_ids: Sequence[int],
//|         in_report_lengths: Sequence[int],
//|         out_report_lengths: Sequence[int]
//|     ) -> None:
//|         """Create a description of a USB HID device. The actual device is created when you
//|         pass a `Device` to `usb_hid.enable()`.
//|
//|         :param ReadableBuffer report_descriptor: The USB HID Report descriptor bytes. The descriptor is not
//|           not verified for correctness; it is up to you to make sure it is not malformed.
//|         :param int usage_page: The Usage Page value from the descriptor. Must match what is in the descriptor.
//|         :param int usage: The Usage value from the descriptor. Must match what is in the descriptor.
//|         :param Sequence[int] report_ids: Sequence of report ids used by the descriptor.
//|           If the ``report_descriptor`` does not specify any report IDs, use ``(0,)``.
//|         :param Sequence[int] in_report_lengths: Sequence of sizes in bytes of the HID reports sent to the host.
//|           The sizes are in order of the ``report_ids``.
//|           Use a size of ``0`` for a report that is not an IN report.
//|           "IN" is with respect to the host.
//|         :param int out_report_lengths: Sequence of sizes in bytes of the HID reports received from the host.
//|           The sizes are in order of the ``report_ids``.
//|           Use a size of ``0`` for a report that is not an OUT report.
//|           "OUT" is with respect to the host.
//|
//|         ``report_ids``, ``in_report_lengths``, and ``out_report_lengths`` must all have the
//|         same number of elements.
//|
//|         Here is an example of a `Device` with a descriptor that specifies two report IDs, 3 and 4.
//|         Report ID 3 sends an IN report of length 5, and receives an OUT report of length 6.
//|         Report ID 4 sends an IN report of length 2, and does not receive an OUT report::
//|
//|             device = usb_hid.Device(
//|                 descriptor=b"...",         # Omitted for brevity.
//|                 report_ids=(3, 4),
//|                 in_report_lengths=(5, 2),
//|                 out_report_lengths=(6, 0),
//|             )
//|         """
//|         ...
//|     KEYBOARD: Device
//|     """Standard keyboard device supporting keycodes 0x00-0xDD, modifiers 0xE-0xE7, and five LED indicators.
//|     Uses Report ID 1 for its IN and OUT reports.
//|     """
//|
//|     MOUSE: Device
//|     """Standard mouse device supporting five mouse buttons, X and Y relative movements from -127 to 127
//|     in each report, and a relative mouse wheel change from -127 to 127 in each report.
//|     Uses Report ID 2 for its IN report.
//|     """
//|
//|     CONSUMER_CONTROL: Device
//|     """Consumer Control device supporting sent values from 1-652, with no rollover.
//|     Uses Report ID 3 for its IN report."""

STATIC mp_obj_t usb_hid_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    usb_hid_device_obj_t *self = mp_obj_malloc(usb_hid_device_obj_t, &usb_hid_device_type);
    enum { ARG_report_descriptor, ARG_usage_page, ARG_usage, ARG_report_ids, ARG_in_report_lengths, ARG_out_report_lengths };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_report_descriptor, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_usage_page, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_usage, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_report_ids, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_in_report_lengths, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_out_report_lengths, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t descriptor_bufinfo;
    mp_get_buffer_raise(args[ARG_report_descriptor].u_obj, &descriptor_bufinfo, MP_BUFFER_READ);
    mp_obj_t descriptor = mp_obj_new_bytearray(descriptor_bufinfo.len, descriptor_bufinfo.buf);

    const mp_int_t usage_page_arg = args[ARG_usage_page].u_int;
    mp_arg_validate_int_range(usage_page_arg, 1, 0xFFFF, MP_QSTR_usage_page);
    const uint16_t usage_page = usage_page_arg;

    const mp_int_t usage_arg = args[ARG_usage].u_int;
    mp_arg_validate_int_range(usage_arg, 1, 0xFFFF, MP_QSTR_usage);
    const uint16_t usage = usage_arg;

    mp_obj_t report_ids = args[ARG_report_ids].u_obj;
    mp_obj_t in_report_lengths = args[ARG_in_report_lengths].u_obj;
    mp_obj_t out_report_lengths = args[ARG_out_report_lengths].u_obj;

    size_t report_ids_count =
        mp_arg_validate_length_min((size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(report_ids)), 1, MP_QSTR_report_ids);

    if ((size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(in_report_lengths)) != report_ids_count ||
        (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(out_report_lengths)) != report_ids_count) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q, %q, and %q must all be the same length"),
            MP_QSTR_report_ids, MP_QSTR_in_report_lengths, MP_QSTR_out_report_lengths);
    }

    uint8_t report_ids_array[report_ids_count];
    uint8_t in_report_lengths_array[report_ids_count];
    uint8_t out_report_lengths_array[report_ids_count];

    // Validate the ids and lengths are all integers in range.
    for (size_t i = 0; i < report_ids_count; i++) {
        mp_obj_t i_obj = MP_OBJ_NEW_SMALL_INT(i);

        report_ids_array[i] = (uint8_t)mp_arg_validate_int_range(
            // It's not the actual argument that's out of range, but its elements.
            // But the error message is close enough.
            MP_OBJ_SMALL_INT_VALUE(mp_obj_subscr(report_ids, i_obj, MP_OBJ_SENTINEL)),
            0, 255, MP_QSTR_report_ids);

        in_report_lengths_array[i] = (uint8_t)mp_arg_validate_int_range(
            MP_OBJ_SMALL_INT_VALUE(mp_obj_subscr(in_report_lengths, i_obj, MP_OBJ_SENTINEL)),
            0, 255, MP_QSTR_in_report_lengths);

        out_report_lengths_array[i] = (uint8_t)mp_arg_validate_int_range(
            MP_OBJ_SMALL_INT_VALUE(mp_obj_subscr(out_report_lengths, i_obj, MP_OBJ_SENTINEL)),
            0, 255, MP_QSTR_out_report_lengths);
    }

    if (report_ids_array[0] == 0 && report_ids_count > 1) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q length must be %d"), MP_QSTR_report_id_space_0, 1);
    }

    common_hal_usb_hid_device_construct(
        self, descriptor, usage_page, usage, report_ids_count, report_ids_array, in_report_lengths_array, out_report_lengths_array);
    return (mp_obj_t)self;
}


//|     def send_report(self, report: ReadableBuffer, report_id: Optional[int] = None) -> None:
//|         """Send an HID report. If the device descriptor specifies zero or one report id's,
//|         you can supply `None` (the default) as the value of ``report_id``.
//|         Otherwise you must specify which report id to use when sending the report.
//|         """
//|         ...
STATIC mp_obj_t usb_hid_device_send_report(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_report, ARG_report_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_report, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_report_id, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_report].u_obj, &bufinfo, MP_BUFFER_READ);

    // -1 asks common_hal to determine the report id if possible.
    mp_int_t report_id_arg = -1;
    if (args[ARG_report_id].u_obj != mp_const_none) {
        report_id_arg = mp_obj_int_get_checked(args[ARG_report_id].u_obj);
    }
    const uint8_t report_id = common_hal_usb_hid_device_validate_report_id(self, report_id_arg);

    common_hal_usb_hid_device_send_report(self, ((uint8_t *)bufinfo.buf), bufinfo.len, report_id);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(usb_hid_device_send_report_obj, 1, usb_hid_device_send_report);

//|     def get_last_received_report(self, report_id: Optional[int] = None) -> Optional[bytes]:
//|         """Get the last received HID OUT or feature report for the given report ID.
//|         The report ID may be omitted if there is no report ID, or only one report ID.
//|         Return `None` if nothing received. After returning a report, subsequent calls
//|         will return `None` until next report is received.
//|         """
//|         ...
STATIC mp_obj_t usb_hid_device_get_last_received_report(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_report_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_report_id, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t report_id_arg = -1;
    if (args[ARG_report_id].u_obj != mp_const_none) {
        report_id_arg = mp_obj_int_get_checked(args[ARG_report_id].u_obj);
    }
    const uint8_t report_id = common_hal_usb_hid_device_validate_report_id(self, report_id_arg);

    return common_hal_usb_hid_device_get_last_received_report(self, report_id);
}
MP_DEFINE_CONST_FUN_OBJ_KW(usb_hid_device_get_last_received_report_obj, 1, usb_hid_device_get_last_received_report);

//|     usage_page: int
//|     """The device usage page identifier, which designates a category of device. (read-only)"""
STATIC mp_obj_t usb_hid_device_obj_get_usage_page(mp_obj_t self_in) {
    usb_hid_device_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_usb_hid_device_get_usage_page(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_hid_device_get_usage_page_obj, usb_hid_device_obj_get_usage_page);

MP_PROPERTY_GETTER(usb_hid_device_usage_page_obj,
    (mp_obj_t)&usb_hid_device_get_usage_page_obj);

//|     usage: int
//|     """The device usage identifier, which designates a specific kind of device. (read-only)
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

MP_PROPERTY_GETTER(usb_hid_device_usage_obj,
    (mp_obj_t)&usb_hid_device_get_usage_obj);

STATIC const mp_rom_map_elem_t usb_hid_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_report),              MP_ROM_PTR(&usb_hid_device_send_report_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_last_received_report), MP_ROM_PTR(&usb_hid_device_get_last_received_report_obj) },
    { MP_ROM_QSTR(MP_QSTR_usage_page),               MP_ROM_PTR(&usb_hid_device_usage_page_obj) },
    { MP_ROM_QSTR(MP_QSTR_usage),                    MP_ROM_PTR(&usb_hid_device_usage_obj) },

    { MP_ROM_QSTR(MP_QSTR_KEYBOARD),                 MP_ROM_PTR(&usb_hid_device_keyboard_obj) },
    { MP_ROM_QSTR(MP_QSTR_MOUSE),                    MP_ROM_PTR(&usb_hid_device_mouse_obj) },
    { MP_ROM_QSTR(MP_QSTR_CONSUMER_CONTROL),         MP_ROM_PTR(&usb_hid_device_consumer_control_obj) },
};

STATIC MP_DEFINE_CONST_DICT(usb_hid_device_locals_dict, usb_hid_device_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    usb_hid_device_type,
    MP_QSTR_Device,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, usb_hid_device_make_new,
    locals_dict, &usb_hid_device_locals_dict
    );
