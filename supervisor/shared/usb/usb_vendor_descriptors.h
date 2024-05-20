// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Ha Thach (tinyusb.org)
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

enum
{
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

size_t vendor_ms_os_20_descriptor_length(void);
uint8_t const *vendor_ms_os_20_descriptor(void);
