/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_MONITOR_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_MONITOR_H

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

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_MONITOR_H
