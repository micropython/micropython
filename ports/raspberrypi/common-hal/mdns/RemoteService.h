// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "lwip/apps/mdns.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t ipv4_address;
    uint16_t port;
    char protocol[5]; // RFC 6763 Section 7.2 - 4 bytes + 1 for NUL
    char service_name[17]; // RFC 6763 Section 7.2 - 16 bytes + 1 for NUL
    char instance_name[64]; // RFC 6763 Section 7.2 - 63 bytes + 1 for NUL
    char hostname[64]; // RFC 6762 Appendix A - 63 bytes for label + 1 for NUL
    mp_obj_t next;
} mdns_remoteservice_obj_t;
