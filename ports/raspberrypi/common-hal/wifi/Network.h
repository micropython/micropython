// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "pico/cyw43_arch.h"

typedef struct {
    mp_obj_base_t base;
    cyw43_ev_scan_result_t record;
} wifi_network_obj_t;
