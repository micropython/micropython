// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"

#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"


void common_hal_ipaddress_ipv4address_construct(ipaddress_ipv4address_obj_t *self, uint8_t *buf, size_t len) {
    self->ip_bytes = mp_obj_new_bytes(buf, len);
}

mp_obj_t common_hal_ipaddress_ipv4address_get_packed(ipaddress_ipv4address_obj_t *self) {
    return self->ip_bytes;
}
