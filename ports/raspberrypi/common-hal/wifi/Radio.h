// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-bindings/wifi/Network.h"

typedef struct {
    mp_obj_base_t base;
    char hostname[254]; // hostname max is 253 chars, + 1 for trailing NUL
    wifi_scannednetworks_obj_t *current_scan;
    uint8_t connected_ssid[32];
    uint8_t connected_ssid_len;
    bool enabled;
} wifi_radio_obj_t;

extern void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self);
