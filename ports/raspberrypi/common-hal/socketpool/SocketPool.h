// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lwip/ip_addr.h"

typedef struct {
    mp_obj_base_t base;
} socketpool_socketpool_obj_t;

void socketpool_resolve_host_raise(socketpool_socketpool_obj_t *self, const char *host, ip_addr_t *addr);
