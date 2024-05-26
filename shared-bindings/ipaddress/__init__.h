// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/ipaddress/__init__.h"

bool ipaddress_parse_ipv4address(const char *ip_str, size_t len, uint32_t *ip_out);

mp_obj_t common_hal_ipaddress_new_ipv4address(uint32_t value);
