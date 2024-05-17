// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_IPADDRESS___INIT___H
#define MICROPY_INCLUDED_SHARED_MODULE_IPADDRESS___INIT___H

#include <stdint.h>

#include "py/obj.h"

mp_obj_t common_hal_ipaddress_new_ipv4(uint32_t value);

#endif // MICROPY_INCLUDED_SHARED_MODULE_IPADDRESS___INIT___H
