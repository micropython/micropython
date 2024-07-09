// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const char *hostname;
    const char *instance_name;
    char default_hostname[sizeof("cpy-XXXXXX")];
    // Track if this object owns access to the underlying MDNS service.
    bool inited;
} mdns_server_obj_t;

void mdns_server_deinit_singleton(void);
