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

#include <string.h>

#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/AuthMode.h"

mp_obj_t common_hal_wifi_network_get_ssid(wifi_network_obj_t *self) {
    const char *cstr = (const char *)self->record.ssid;
    return mp_obj_new_str(cstr, strlen(cstr));
}

#define MAC_ADDRESS_LENGTH 6

mp_obj_t common_hal_wifi_network_get_bssid(wifi_network_obj_t *self) {
    return mp_obj_new_bytes(self->record.bssid, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_network_get_rssi(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->record.rssi);
}

mp_obj_t common_hal_wifi_network_get_channel(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->record.primary);
}

mp_obj_t common_hal_wifi_network_get_country(wifi_network_obj_t *self) {
    const char *cstr = (const char *)self->record.country.cc;
    // 2 instead of strlen(cstr) as this gives us only the country-code
    return mp_obj_new_str(cstr, 2);
}

mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self) {
    uint8_t authmode_mask = 0;
    switch (self->record.authmode) {
        case WIFI_AUTH_OPEN:
            authmode_mask = (1 << AUTHMODE_OPEN);
            break;
        case WIFI_AUTH_WEP:
            authmode_mask = (1 << AUTHMODE_WEP);
            break;
        case WIFI_AUTH_WPA_PSK:
            authmode_mask = (1 << AUTHMODE_WPA) | (1 << AUTHMODE_PSK);
            break;
        case WIFI_AUTH_WPA2_PSK:
            authmode_mask = (1 << AUTHMODE_WPA2) | (1 << AUTHMODE_PSK);
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            authmode_mask = (1 << AUTHMODE_WPA) | (1 << AUTHMODE_WPA2) | (1 << AUTHMODE_PSK);
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            authmode_mask = (1 << AUTHMODE_WPA2) | (1 << AUTHMODE_ENTERPRISE);
            break;
        case WIFI_AUTH_WPA3_PSK:
            authmode_mask = (1 << AUTHMODE_WPA3) | (1 << AUTHMODE_PSK);
            break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
            authmode_mask = (1 << AUTHMODE_WPA2) | (1 << AUTHMODE_WPA3) | (1 << AUTHMODE_PSK);
            break;
        default:
            break;
    }
    mp_obj_t authmode_list = mp_obj_new_list(0, NULL);
    if (authmode_mask != 0) {
        for (uint8_t i = 0; i < 8; i++) {
            if ((authmode_mask >> i) & 1) {
                mp_obj_list_append(authmode_list, cp_enum_find(&wifi_authmode_type, i));
            }
        }
    }
    return authmode_list;
}
