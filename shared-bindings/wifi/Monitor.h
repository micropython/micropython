// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/wifi/Monitor.h"

extern const mp_obj_type_t wifi_monitor_type;

void common_hal_wifi_monitor_construct(wifi_monitor_obj_t *self,
    uint8_t channel, size_t queue);
void common_hal_wifi_monitor_deinit(wifi_monitor_obj_t *self);
bool common_hal_wifi_monitor_deinited(void);

void common_hal_wifi_monitor_set_channel(wifi_monitor_obj_t *self, uint8_t channel);
mp_obj_t common_hal_wifi_monitor_get_channel(wifi_monitor_obj_t *self);

mp_obj_t common_hal_wifi_monitor_get_queue(wifi_monitor_obj_t *self);

mp_obj_t common_hal_wifi_monitor_get_lost(wifi_monitor_obj_t *self);

mp_obj_t common_hal_wifi_monitor_get_queued(wifi_monitor_obj_t *self);

mp_obj_t common_hal_wifi_monitor_get_packet(wifi_monitor_obj_t *self);
