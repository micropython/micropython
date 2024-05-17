// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
    uint32_t authmode_mask = 0;
    switch (self->record.authmode) {
        case WIFI_AUTH_OPEN:
            authmode_mask = AUTHMODE_OPEN;
            break;
        case WIFI_AUTH_WEP:
            authmode_mask = AUTHMODE_WEP;
            break;
        case WIFI_AUTH_WPA_PSK:
            authmode_mask = AUTHMODE_WPA | AUTHMODE_PSK;
            break;
        case WIFI_AUTH_WPA2_PSK:
            authmode_mask = AUTHMODE_WPA2 | AUTHMODE_PSK;
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            authmode_mask = AUTHMODE_WPA | AUTHMODE_WPA2 | AUTHMODE_PSK;
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            authmode_mask = AUTHMODE_WPA2 | AUTHMODE_ENTERPRISE;
            break;
        case WIFI_AUTH_WPA3_PSK:
            authmode_mask = AUTHMODE_WPA3 | AUTHMODE_PSK;
            break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
            authmode_mask = AUTHMODE_WPA2 | AUTHMODE_WPA3 | AUTHMODE_PSK;
            break;
        default:
            break;
    }
    mp_obj_t authmode_list = mp_obj_new_list(0, NULL);
    if (authmode_mask != 0) {
        for (uint8_t i = 0; i < 32; i++) {
            if ((authmode_mask >> i) & 1) {
                mp_obj_list_append(authmode_list, cp_enum_find(&wifi_authmode_type, 1 << i));
            }
        }
    }
    return authmode_list;
}
