// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_USB_UTF16LE_H
#define MICROPY_INCLUDED_SHARED_MODULE_USB_UTF16LE_H

#include "py/obj.h"

mp_obj_t utf16le_to_string(const uint16_t *buf, size_t utf16_len);

#endif // MICROPY_INCLUDED_SHARED_MODULE_USB_UTF16LE_H
