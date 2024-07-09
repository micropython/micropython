// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objarray.h"

#include "shared-module/usb_hid/Device.h"

extern const mp_obj_type_t usb_hid_device_type;

void common_hal_usb_hid_device_construct(usb_hid_device_obj_t *self, mp_obj_t report_descriptor, uint16_t usage_page, uint16_t usage, size_t report_ids_count, uint8_t *report_ids, uint8_t *in_report_lengths, uint8_t *out_report_lengths);
void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t *report, uint8_t len, uint8_t report_id);
mp_obj_t common_hal_usb_hid_device_get_last_received_report(usb_hid_device_obj_t *self, uint8_t report_id);
uint16_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self);
uint16_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self);
uint8_t common_hal_usb_hid_device_validate_report_id(usb_hid_device_obj_t *self, mp_int_t report_id);
