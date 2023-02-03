/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
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

#pragma once

#include "py/obj.h"
#include "py/ringbuf.h"

#include "bindings/espnow/ESPNowStats.h"
#include "bindings/espnow/Peers.h"

#include "esp_wifi.h"

// The data structure for the espnow_singleton.
typedef struct _espnow_obj_t {
    mp_obj_base_t base;
    ringbuf_t *recv_buffer;         // A buffer for received packets
    size_t recv_buffer_size;        // The size of the recv_buffer
    wifi_phy_rate_t phy_rate;       // The ESP-NOW physical layer rate.
    espnow_stats_obj_t *tx_stats;   // The tx packet stats
    espnow_stats_obj_t *rx_stats;   // The rx packet stats
    espnow_peers_obj_t *peers;      // The sequence of peers
} espnow_obj_t;

extern void espnow_reset(void);

extern void common_hal_espnow_construct(espnow_obj_t *self, mp_int_t buffer_size, mp_int_t phy_rate);
extern void common_hal_espnow_init(espnow_obj_t *self);
extern void common_hal_espnow_deinit(espnow_obj_t *self);
extern bool common_hal_espnow_deinited(espnow_obj_t *self);

extern void common_hal_espnow_set_buffer_size(espnow_obj_t *self, mp_int_t value);
extern void common_hal_espnow_set_phy_rate(espnow_obj_t *self, mp_int_t value);
extern void common_hal_espnow_set_pmk(espnow_obj_t *self, const uint8_t *key);

extern mp_obj_t common_hal_espnow_send(espnow_obj_t *self, const uint8_t *mac, const mp_buffer_info_t *message);
extern mp_obj_t common_hal_espnow_recv(espnow_obj_t *self);
