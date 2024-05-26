// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lwip/apps/mdns_opts.h"

#define MDNS_MAX_TXT_RECORDS 32

typedef struct {
    mp_obj_base_t base;
    const char *hostname;
    const char *instance_name;
    char default_hostname[sizeof("cpy-XXXXXX")];
    const char *service_type[MDNS_MAX_SERVICES];
    size_t num_txt_records;
    const char *txt_records[MDNS_MAX_TXT_RECORDS];
    // Track if this object owns access to the underlying MDNS service.
    bool inited;
} mdns_server_obj_t;
