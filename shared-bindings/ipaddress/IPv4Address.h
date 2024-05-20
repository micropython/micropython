// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/ipaddress/IPv4Address.h"

extern const mp_obj_type_t ipaddress_ipv4address_type;

mp_obj_t common_hal_ipaddress_new_ipv4address(uint32_t value);
void common_hal_ipaddress_ipv4address_construct(ipaddress_ipv4address_obj_t *self, uint8_t *buf, size_t len);
mp_obj_t common_hal_ipaddress_ipv4address_get_packed(ipaddress_ipv4address_obj_t *self);
