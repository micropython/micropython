// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"
#include "py/objtuple.h"
#include "supervisor/usb.h"

bool usb_cdc_console_enabled(void);
bool usb_cdc_data_enabled(void);

void usb_cdc_set_defaults(void);

size_t usb_cdc_descriptor_length(void);
size_t usb_cdc_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string, bool console);

#if CIRCUITPY_USB_VENDOR
bool usb_vendor_enabled(void);
size_t usb_vendor_descriptor_length(void);
size_t usb_vendor_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string);

size_t vendor_ms_os_20_descriptor_length(void);
uint8_t const *vendor_ms_os_20_descriptor(void);
#endif
