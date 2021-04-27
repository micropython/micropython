/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#ifndef SHARED_MODULE_USB_HID_DEVICE_H
#define SHARED_MODULE_USB_HID_DEVICE_H

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"

typedef struct  {
    mp_obj_base_t base;
    // If not MP_OBJ_NULL, points to Python array object whose contents are the descriptor.
    mp_obj_t descriptor_obj;
    // If not NULL, points to raw bytes that are the descriptor.
    uint8_t *descriptor;
    uint8_t *in_report_buffer;
    uint8_t *out_report_buffer;
    uint16_t report_id_index;
    uint16_t descriptor_length;
    uint8_t usage_page;
    uint8_t usage;
    uint8_t report_id;
    uint8_t in_report_length;
    uint8_t out_report_length;
} usb_hid_device_obj_t;

extern usb_hid_device_obj_t usb_hid_device_keyboard_obj;
extern usb_hid_device_obj_t usb_hid_device_mouse_obj;
extern usb_hid_device_obj_t usb_hid_device_consumer_control_obj;

#endif /* SHARED_MODULE_USB_HID_DEVICE_H */
