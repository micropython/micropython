// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WIFI___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WIFI___INIT___H

#include "shared-bindings/wifi/Radio.h"

extern wifi_radio_obj_t common_hal_wifi_radio_obj;

void common_hal_wifi_init(bool user_initiated);
void common_hal_wifi_gc_collect(void);

void wifi_user_reset(void);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI___INIT___H
