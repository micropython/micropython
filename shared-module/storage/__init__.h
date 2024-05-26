// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if CIRCUITPY_USB_DEVICE
#include "supervisor/usb.h"

bool storage_usb_enabled(void);
void storage_usb_set_defaults(void);
size_t storage_usb_descriptor_length(void);
size_t storage_usb_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string);
#endif
