// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"
#include "cyw43.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t results;
    bool done;
} wifi_scannednetworks_obj_t;

void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self);
void wifi_scannednetworks_start_scan(wifi_scannednetworks_obj_t *self);
