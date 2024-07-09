// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"

typedef struct  {
    mp_obj_base_t base;
    // Python buffer object whose contents are the descriptor.
    const uint8_t *report_descriptor;
    uint8_t *in_report_buffers[CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR];
    uint8_t *out_report_buffers[CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR];
    uint8_t out_report_buffers_updated[CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR];
    uint16_t report_descriptor_length;
    uint8_t report_ids[CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR];
    uint8_t in_report_lengths[CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR];
    uint8_t out_report_lengths[CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR];
    uint16_t usage_page;
    uint16_t usage;
    uint8_t num_report_ids;
} usb_hid_device_obj_t;

extern const usb_hid_device_obj_t usb_hid_device_keyboard_obj;
extern const usb_hid_device_obj_t usb_hid_device_mouse_obj;
extern const usb_hid_device_obj_t usb_hid_device_consumer_control_obj;

void usb_hid_device_create_report_buffers(usb_hid_device_obj_t *self);

extern char *custom_usb_hid_interface_name;
