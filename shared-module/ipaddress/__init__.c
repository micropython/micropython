// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"

mp_obj_t common_hal_ipaddress_new_ipv4address(uint32_t value) {
    ipaddress_ipv4address_obj_t *self = mp_obj_malloc(ipaddress_ipv4address_obj_t, &ipaddress_ipv4address_type);
    common_hal_ipaddress_ipv4address_construct(self, (uint8_t *)&value, 4);
    return MP_OBJ_FROM_PTR(self);
}
