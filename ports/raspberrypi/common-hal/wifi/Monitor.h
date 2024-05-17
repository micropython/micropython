// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WIFI_MONITOR_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WIFI_MONITOR_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t channel;
    size_t lost;
    size_t queue_length;
} wifi_monitor_obj_t;

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WIFI_MONITOR_H
