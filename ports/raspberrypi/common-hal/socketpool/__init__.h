// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "lwip/ip_addr.h"

mp_obj_t socketpool_ip_addr_to_str(const ip_addr_t *addr);
void socketpool_resolve_host_raise(const char *host, ip_addr_t *addr);
