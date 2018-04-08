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

#include "genhdr/autogen_usb_descriptor.h"

// Buffers are report size + 1 to include the Report ID prefix byte if needed.
static uint8_t keyboard_report_buffer[USB_HID_REPORT_LENGTH_KEYBOARD + 1];
static uint8_t mouse_report_buffer[USB_HID_REPORT_LENGTH_MOUSE + 1];
static uint8_t consumer_report_buffer[USB_HID_REPORT_LENGTH_CONSUMER + 1];
static uint8_t sys_control_report_buffer[USB_HID_REPORT_LENGTH_SYS_CONTROL + 1];

usb_hid_device_obj_t usb_hid_devices[USB_HID_NUM_DEVICES] = {
    {
        .base = { .type = &usb_hid_device_type },
        .report_buffer = keyboard_report_buffer,
        .endpoint = USB_HID_ENDPOINT_IN,
        .report_id = USB_HID_REPORT_ID_KEYBOARD,
        .report_length = USB_HID_REPORT_LENGTH_KEYBOARD,
        .usage_page = 0x01,
        .usage = 0x06,
    },
    {
        .base = { .type = &usb_hid_device_type },
        .report_buffer = mouse_report_buffer,
        .endpoint = USB_HID_ENDPOINT_IN,
        .report_id = USB_HID_REPORT_ID_MOUSE,
        .report_length = USB_HID_REPORT_LENGTH_MOUSE,
        .usage_page = 0x01,
        .usage = 0x02,
    },
    {
        .base = { .type = &usb_hid_device_type },
        .report_buffer = consumer_report_buffer,
        .endpoint = USB_HID_ENDPOINT_IN,
        .report_id = USB_HID_REPORT_ID_CONSUMER,
        .report_length = USB_HID_REPORT_LENGTH_CONSUMER,
        .usage_page = 0x0C,
        .usage = 0x01,
    },
    {
        .base = { .type = &usb_hid_device_type },
        .report_buffer = sys_control_report_buffer,
        .endpoint = USB_HID_ENDPOINT_IN,
        .report_id = USB_HID_REPORT_ID_SYS_CONTROL,
        .report_length = USB_HID_REPORT_LENGTH_SYS_CONTROL,
        .usage_page = 0x01,
        .usage = 0x80,
    },
};


mp_obj_tuple_t common_hal_usb_hid_devices = {
    .base = {
        .type = &mp_type_tuple,
    },
    .len = USB_HID_NUM_DEVICES,
    .items = {
        (mp_obj_t) &usb_hid_devices[0],
        (mp_obj_t) &usb_hid_devices[1],
        (mp_obj_t) &usb_hid_devices[2],
        (mp_obj_t) &usb_hid_devices[3],
    }
};
