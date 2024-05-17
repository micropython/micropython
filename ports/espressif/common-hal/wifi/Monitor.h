// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WIFI_MONITOR_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WIFI_MONITOR_H

#include "py/obj.h"
#include "components/esp_wifi/include/esp_wifi.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t channel;
    size_t lost;
    size_t queue_length;
    QueueHandle_t queue;
} wifi_monitor_obj_t;

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WIFI_MONITOR_H
