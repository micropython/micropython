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
    return mp_obj_new_str(cstr, self->record.ssid_len);
}

#define MAC_ADDRESS_LENGTH 6

mp_obj_t common_hal_wifi_network_get_bssid(wifi_network_obj_t *self) {
    return mp_obj_new_bytes(self->record.bssid, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_network_get_rssi(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->record.rssi);
}

mp_obj_t common_hal_wifi_network_get_channel(wifi_network_obj_t *self) {
    return mp_obj_new_int(self->record.channel);
}

mp_obj_t common_hal_wifi_network_get_country(wifi_network_obj_t *self) {
    return (mp_obj_t)MP_QSTR_;
}

mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self) {
    uint32_t authmode_mask = 0;
    if (self->record.auth_mode == 0) {
        authmode_mask = AUTHMODE_OPEN;
    }
    if (self->record.auth_mode & 1) {
        authmode_mask |= AUTHMODE_PSK;
    }
    ;
    if (self->record.auth_mode & 2) {
        authmode_mask |= AUTHMODE_WPA;
    }
    ;
    if (self->record.auth_mode & 4) {
        authmode_mask |= AUTHMODE_WPA2;
    }
    ;
    mp_obj_t authmode_list = mp_obj_new_list(0, NULL);
    if (authmode_mask != 0) {
        for (uint32_t i = 0; i < 32; i++) {
            if ((authmode_mask >> i) & 1) {
                mp_obj_list_append(authmode_list, cp_enum_find(&wifi_authmode_type, 1 << i));
            }
        }
    }
    return authmode_list;
}
