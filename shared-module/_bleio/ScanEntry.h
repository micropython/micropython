// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objstr.h"
#include "shared-bindings/_bleio/Address.h"

typedef struct {
    mp_obj_base_t base;
    bool connectable;
    bool scan_response;
    int8_t rssi;
    bleio_address_obj_t *address;
    mp_obj_str_t *data;
    uint64_t time_received;
} bleio_scanentry_obj_t;

bool bleio_scanentry_data_matches(const uint8_t *data, size_t len, const uint8_t *prefixes, size_t prefix_length, bool any);
