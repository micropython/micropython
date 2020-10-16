/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_RADIO_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_RADIO_H

#include <stdint.h>

#include "common-hal/wifi/Radio.h"

#include "py/objstr.h"

const mp_obj_type_t wifi_radio_type;


typedef enum {
    WIFI_RADIO_ERROR_NONE,
    WIFI_RADIO_ERROR_UNKNOWN,
    WIFI_RADIO_ERROR_AUTH,
    WIFI_RADIO_ERROR_NO_AP_FOUND
} wifi_radio_error_t;

extern bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled);

extern mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self);

extern mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self);

extern wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t* ssid, size_t ssid_len, uint8_t* password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t* bssid, size_t bssid_len);

extern mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_dns(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_gateway(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_subnet(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self);

extern mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_RADIO_H
