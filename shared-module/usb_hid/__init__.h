// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_hid/Device.h"
#include "supervisor/usb.h"

extern usb_hid_device_obj_t usb_hid_devices[];

bool usb_hid_enabled(void);
uint8_t usb_hid_boot_device(void);
void usb_hid_set_defaults(void);

size_t usb_hid_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string, uint16_t report_descriptor_length, uint8_t boot_device);
size_t usb_hid_descriptor_length(void);
size_t usb_hid_report_descriptor_length(void);

void usb_hid_setup_devices(void);
size_t usb_hid_report_descriptor_length(void);
void usb_hid_build_report_descriptor(void);

bool usb_hid_get_device_with_report_id(uint8_t report_id, usb_hid_device_obj_t **device_out, size_t *id_idx_out);

void usb_hid_gc_collect(void);
