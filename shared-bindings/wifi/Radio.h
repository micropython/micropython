// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "common-hal/wifi/Radio.h"

#include "py/objstr.h"
#include "py/objnamedtuple.h"

extern const mp_obj_type_t wifi_radio_type;
extern const mp_obj_namedtuple_type_t wifi_radio_station_type;

typedef enum {
    // 0 is circuitpython-specific; 1-53 are IEEE; 200+ are Espressif
    // See wifi_err_reason_t in esp-idf/components/esp_wifi/include/esp_wifi_types.h
    WIFI_RADIO_ERROR_NONE                        = 0,
    WIFI_RADIO_ERROR_UNSPECIFIED                 = 1,
    WIFI_RADIO_ERROR_AUTH_EXPIRE                 = 2,
    WIFI_RADIO_ERROR_AUTH_LEAVE                  = 3,
    WIFI_RADIO_ERROR_ASSOC_EXPIRE                = 4,
    WIFI_RADIO_ERROR_ASSOC_TOOMANY               = 5,
    WIFI_RADIO_ERROR_NOT_AUTHED                  = 6,
    WIFI_RADIO_ERROR_NOT_ASSOCED                 = 7,
    WIFI_RADIO_ERROR_ASSOC_LEAVE                 = 8,
    WIFI_RADIO_ERROR_ASSOC_NOT_AUTHED            = 9,
    WIFI_RADIO_ERROR_DISASSOC_PWRCAP_BAD         = 10,
    WIFI_RADIO_ERROR_DISASSOC_SUPCHAN_BAD        = 11,
    WIFI_RADIO_ERROR_IE_INVALID                  = 13,
    WIFI_RADIO_ERROR_MIC_FAILURE                 = 14,
    WIFI_RADIO_ERROR_4WAY_HANDSHAKE_TIMEOUT      = 15,
    WIFI_RADIO_ERROR_GROUP_KEY_UPDATE_TIMEOUT    = 16,
    WIFI_RADIO_ERROR_IE_IN_4WAY_DIFFERS          = 17,
    WIFI_RADIO_ERROR_GROUP_CIPHER_INVALID        = 18,
    WIFI_RADIO_ERROR_PAIRWISE_CIPHER_INVALID     = 19,
    WIFI_RADIO_ERROR_AKMP_INVALID                = 20,
    WIFI_RADIO_ERROR_UNSUPP_RSN_IE_VERSION       = 21,
    WIFI_RADIO_ERROR_INVALID_RSN_IE_CAP          = 22,
    WIFI_RADIO_ERROR_802_1X_AUTH_FAILED          = 23,
    WIFI_RADIO_ERROR_CIPHER_SUITE_REJECTED       = 24,
    WIFI_RADIO_ERROR_INVALID_PMKID               = 53,
    WIFI_RADIO_ERROR_BEACON_TIMEOUT              = 200,
    WIFI_RADIO_ERROR_NO_AP_FOUND                 = 201,
    WIFI_RADIO_ERROR_AUTH_FAIL                   = 202,
    WIFI_RADIO_ERROR_ASSOC_FAIL                  = 203,
    WIFI_RADIO_ERROR_HANDSHAKE_TIMEOUT           = 204,
    WIFI_RADIO_ERROR_CONNECTION_FAIL             = 205,
    WIFI_RADIO_ERROR_AP_TSF_RESET                = 206,
    WIFI_RADIO_ERRROR_ROAMING                    = 207,
    WIFI_RADIO_ASSOC_COMEBACK_TOO_LONG           = 208,
    WIFI_RADIO_SA_QUERY_TIMEOUT                  = 209,
    WIFI_RADIO_NO_AP_FOUND_W_COMPATIBLE_SECURITY = 210,  // collapsed to AUTH_FAIL
    WIFI_RADIO_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD = 211,  // collapsed to AUTH_FAIL
    WIFI_RADIO_NO_AP_FOUND_IN_RSSI_THRESHOLD     = 212,
} wifi_radio_error_t;

extern bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled);

extern mp_obj_t common_hal_wifi_radio_get_hostname(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_hostname(wifi_radio_obj_t *self, const char *hostname);


extern void wifi_radio_get_mac_address(wifi_radio_obj_t *self, uint8_t *mac);
extern mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_mac_address(wifi_radio_obj_t *self, const uint8_t *mac);
extern mp_obj_t common_hal_wifi_radio_get_mac_address_ap(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_mac_address_ap(wifi_radio_obj_t *self, const uint8_t *mac);

extern mp_float_t common_hal_wifi_radio_get_tx_power(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_tx_power(wifi_radio_obj_t *self, const mp_float_t power);

extern void common_hal_wifi_radio_set_listen_interval(wifi_radio_obj_t *self, const mp_int_t listen_interval);
extern mp_int_t common_hal_wifi_radio_get_listen_interval(wifi_radio_obj_t *self);

extern mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self, uint8_t start_channel, uint8_t stop_channel);
extern void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self);

extern void common_hal_wifi_radio_start_station(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_stop_station(wifi_radio_obj_t *self);

extern void common_hal_wifi_radio_start_ap(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, uint32_t authmode, uint8_t max_connections);
extern void common_hal_wifi_radio_stop_ap(wifi_radio_obj_t *self);
extern bool common_hal_wifi_radio_get_ap_active(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_stations_ap(wifi_radio_obj_t *self);

extern void common_hal_wifi_radio_start_dhcp_client(wifi_radio_obj_t *self, bool ipv4, bool ipv6);
extern void common_hal_wifi_radio_stop_dhcp_client(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_start_dhcp_server(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_stop_dhcp_server(wifi_radio_obj_t *self);

extern wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t *bssid, size_t bssid_len);
extern bool common_hal_wifi_radio_get_connected(wifi_radio_obj_t *self);

extern mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_dns(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_ipv4_dns(wifi_radio_obj_t *self, mp_obj_t ipv4_dns_addr);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_gateway(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_gateway_ap(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_subnet(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_subnet_ap(wifi_radio_obj_t *self);
uint32_t wifi_radio_get_ipv4_address(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self);
extern mp_obj_t common_hal_wifi_radio_get_ipv4_address_ap(wifi_radio_obj_t *self);

mp_obj_t common_hal_wifi_radio_get_addresses(wifi_radio_obj_t *self);
mp_obj_t common_hal_wifi_radio_get_addresses_ap(wifi_radio_obj_t *self);

extern mp_obj_t common_hal_wifi_radio_get_dns(wifi_radio_obj_t *self);
extern void common_hal_wifi_radio_set_dns(wifi_radio_obj_t *self, mp_obj_t dns_addr);

extern void common_hal_wifi_radio_set_ipv4_address(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns_addr);
extern void common_hal_wifi_radio_set_ipv4_address_ap(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway);

extern mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout);
