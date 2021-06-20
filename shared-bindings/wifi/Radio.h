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
    // 0 is circuitpython-specific; 1-53 are IEEE; 200+ are Espressif
    WIFI_RADIO_ERROR_NONE                     = 0,
    WIFI_RADIO_ERROR_UNSPECIFIED              = 1,
    WIFI_RADIO_ERROR_AUTH_EXPIRE              = 2,
    WIFI_RADIO_ERROR_AUTH_LEAVE               = 3,
    WIFI_RADIO_ERROR_ASSOC_EXPIRE             = 4,
    WIFI_RADIO_ERROR_ASSOC_TOOMANY            = 5,
    WIFI_RADIO_ERROR_NOT_AUTHED               = 6,
    WIFI_RADIO_ERROR_NOT_ASSOCED              = 7,
    WIFI_RADIO_ERROR_ASSOC_LEAVE              = 8,
    WIFI_RADIO_ERROR_ASSOC_NOT_AUTHED         = 9,
    WIFI_RADIO_ERROR_DISASSOC_PWRCAP_BAD      = 10,
    WIFI_RADIO_ERROR_DISASSOC_SUPCHAN_BAD     = 11,
    WIFI_RADIO_ERROR_IE_INVALID               = 13,
    WIFI_RADIO_ERROR_MIC_FAILURE              = 14,
    WIFI_RADIO_ERROR_4WAY_HANDSHAKE_TIMEOUT   = 15,
    WIFI_RADIO_ERROR_GROUP_KEY_UPDATE_TIMEOUT = 16,
    WIFI_RADIO_ERROR_IE_IN_4WAY_DIFFERS       = 17,
    WIFI_RADIO_ERROR_GROUP_CIPHER_INVALID     = 18,
    WIFI_RADIO_ERROR_PAIRWISE_CIPHER_INVALID  = 19,
    WIFI_RADIO_ERROR_AKMP_INVALID             = 20,
    WIFI_RADIO_ERROR_UNSUPP_RSN_IE_VERSION    = 21,
    WIFI_RADIO_ERROR_INVALID_RSN_IE_CAP       = 22,
    WIFI_RADIO_ERROR_802_1X_AUTH_FAILED       = 23,
    WIFI_RADIO_ERROR_CIPHER_SUITE_REJECTED    = 24,
    WIFI_RADIO_ERROR_INVALID_PMKID            = 53,
    WIFI_RADIO_ERROR_BEACON_TIMEOUT           = 200,
    WIFI_RADIO_ERROR_NO_AP_FOUND              = 201,
    WIFI_RADIO_ERROR_AUTH_FAIL                = 202,
    WIFI_RADIO_ERROR_ASSOC_FAIL               = 203,
    WIFI_RADIO_ERROR_HANDSHAKE_TIMEOUT        = 204,
    WIFI_RADIO_ERROR_CONNECTION_FAIL          = 205,
    WIFI_RADIO_ERROR_AP_TSF_RESET             = 206,
} wifi_radio_error_t;

extern bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled);

extern mp_obj_t common_hal_wifi_radio_get_hostname(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_hostname(wifi_radio_obj_t *self, const char *hostname);

extern mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_mac_address_ap(wifi_radio_obj_t *self);

extern mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self);

extern void common_hal_wifi_radio_start_station(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_stop_station(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_start_ap(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, uint8_t authmode);
extern void common_hal_wifi_radio_stop_ap(wifi_radio_obj_t *self);

extern wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t *bssid, size_t bssid_len);

extern mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_dns(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_gateway(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_gateway_ap(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_subnet(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_subnet_ap(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_address_ap(wifi_radio_obj_t *self);

extern mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_RADIO_H
