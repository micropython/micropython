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

#include "common-hal/usb_hid/Device.h"

#include "shared-bindings/usb_hid/Device.h"

#include "tools/autogen_usb_descriptor.h"

static uint8_t mouse_report_buffer[UDI_HID_MOUSE_REPORT_SIZE];
static uint8_t kbd_report_buffer[UDI_HID_KBD_REPORT_SIZE];

usb_hid_device_obj_t usb_hid_devices[2] = {
    {
        .kind = USB_HID_MOUSE,
        .report_length = UDI_HID_MOUSE_REPORT_SIZE,
        .report_buffer = mouse_report_buffer,
        .usage_page = 0x01,
        .usage = 0x02,
    },
    {
        .kind = USB_HID_KEYBOARD,
        .report_length = UDI_HID_KBD_REPORT_SIZE,
        .report_buffer = kbd_report_buffer,
        .usage_page = 0x01,
        .usage = 0x06,
    }
};

// TODO(tannewt): Make this a mp_obj_tuple_t when it is dynamically allocated.
// until then we hard code it to two entries so LTO is happy.
mp_obj_tuple2_t common_hal_usb_hid_devices = {
    .base = {
        .type = &mp_type_tuple,
    },
    .len = 2,
    .items = {
        (mp_obj_t) &usb_hid_devices[USB_HID_DEVICE_MOUSE],
        (mp_obj_t) &usb_hid_devices[USB_HID_DEVICE_KEYBOARD],
    }
};
