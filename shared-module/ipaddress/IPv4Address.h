// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_IPADDRESS_IPV4ADDRESS_H
#define MICROPY_INCLUDED_SHARED_MODULE_IPADDRESS_IPV4ADDRESS_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t ip_bytes;
} ipaddress_ipv4address_obj_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_IPADDRESS_IPV4ADDRESS_H
