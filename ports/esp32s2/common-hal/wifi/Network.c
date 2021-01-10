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

#include "shared-bindings/wifi/Network.h"

#include <string.h>

#include "py/obj.h"

mp_obj_t common_hal_wifi_network_get_ssid(wifi_network_obj_t *self) {
    const char* cstr = (const char*) self->record.ssid;
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
    const char* cstr = (const char*) self->record.country.cc;
        // 2 instead of strlen(cstr) as this gives us only the country-code
        return mp_obj_new_str(cstr, 2);
}

mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self) {
    const char* authmode = "";
    switch (self->record.authmode) {
        case WIFI_AUTH_OPEN:
            authmode = "OPEN";
            break;
        case WIFI_AUTH_WEP:
            authmode = "WEP";
            break;
        case WIFI_AUTH_WPA_PSK:
            authmode = "WPA_PSK";
            break;
        case WIFI_AUTH_WPA2_PSK:
            authmode = "WPA2_PSK";
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            authmode = "WPA_WPA2_PSK";
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            authmode = "WPA2_ENTERPRISE";
            break;
        case WIFI_AUTH_WPA3_PSK:
            authmode = "WPA3_PSK";
            break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
            authmode = "WPA2_WPA3_PSK";
            break;
        default:
            authmode = "UNKNOWN";
            break;
    }
        return mp_obj_new_str(authmode, strlen(authmode));
}
