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

#include "bindings/espnow/Peers.h"

#include "esp_wifi.h"

typedef struct _espnow_obj_t {
    mp_obj_base_t base;
    ringbuf_t *recv_buffer;
    size_t recv_buffer_size;
    wifi_phy_rate_t phy_rate;
    espnow_peers_obj_t *peers;
    volatile size_t send_success;
    volatile size_t send_failure;
    volatile size_t read_success;
    volatile size_t read_failure;
} espnow_obj_t;

extern void espnow_reset(void);

extern void common_hal_espnow_construct(espnow_obj_t *self, mp_int_t buffer_size, mp_int_t phy_rate);
extern void common_hal_espnow_init(espnow_obj_t *self);
extern void common_hal_espnow_deinit(espnow_obj_t *self);
extern bool common_hal_espnow_deinited(espnow_obj_t *self);

extern void common_hal_espnow_set_phy_rate(espnow_obj_t *self, mp_int_t value);
extern void common_hal_espnow_set_pmk(espnow_obj_t *self, const uint8_t *key);

extern mp_obj_t common_hal_espnow_send(espnow_obj_t *self, const mp_buffer_info_t *message, const uint8_t *mac);
extern mp_obj_t common_hal_espnow_read(espnow_obj_t *self);
