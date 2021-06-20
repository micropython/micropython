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

#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/Network.h"

#include <string.h>

#include "common-hal/wifi/__init__.h"
#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-module/ipaddress/__init__.h"

#include "components/esp_wifi/include/esp_wifi.h"
#include "components/lwip/include/apps/ping/ping_sock.h"

#define MAC_ADDRESS_LENGTH 6

static void set_mode_station(wifi_radio_obj_t *self, bool state) {
    wifi_mode_t next_mode;
    if (state) {
        if (self->ap_mode) {
            next_mode = WIFI_MODE_APSTA;
        } else {
            next_mode = WIFI_MODE_STA;
        }
    } else {
        if (self->ap_mode) {
            next_mode = WIFI_MODE_AP;
        } else {
            next_mode = WIFI_MODE_NULL;
        }
    }
    esp_wifi_set_mode(next_mode);
    self->sta_mode = state;
}

static void set_mode_ap(wifi_radio_obj_t *self, bool state) {
    wifi_mode_t next_mode;
    if (state) {
        if (self->sta_mode) {
            next_mode = WIFI_MODE_APSTA;
        } else {
            next_mode = WIFI_MODE_AP;
        }
    } else {
        if (self->sta_mode) {
            next_mode = WIFI_MODE_STA;
        } else {
            next_mode = WIFI_MODE_NULL;
        }
    }
    esp_wifi_set_mode(next_mode);
    self->ap_mode = state;
}

bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self) {
    return self->started;
}

void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled) {
    if (self->started && !enabled) {
        if (self->current_scan != NULL) {
            common_hal_wifi_radio_stop_scanning_networks(self);
        }
        ESP_ERROR_CHECK(esp_wifi_stop());
        self->started = false;
        return;
    }
    if (!self->started && enabled) {
        ESP_ERROR_CHECK(esp_wifi_start());
        self->started = true;
        return;
    }
}

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
    uint8_t mac[MAC_ADDRESS_LENGTH];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_radio_get_mac_address_ap(wifi_radio_obj_t *self) {
    uint8_t mac[MAC_ADDRESS_LENGTH];
    esp_wifi_get_mac(ESP_IF_WIFI_AP, mac);
    return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self) {
    if (self->current_scan != NULL) {
        mp_raise_RuntimeError(translate("Already scanning for wifi networks"));
    }
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(translate("wifi is not enabled"));
    }
    set_mode_station(self, true);

    wifi_scannednetworks_obj_t *scan = m_new_obj(wifi_scannednetworks_obj_t);
    scan->base.type = &wifi_scannednetworks_type;
    self->current_scan = scan;
    scan->start_channel = 1;
    scan->end_channel = 11;
    scan->radio_event_group = self->event_group_handle;
    wifi_scannednetworks_scan_next_channel(scan);
    return scan;
}

void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self) {
    // Return early if self->current_scan is NULL to avoid hang
    if (self->current_scan == NULL) {
        return;
    }
    // Free the memory used to store the found aps.
    wifi_scannednetworks_deinit(self->current_scan);
    self->current_scan = NULL;
}

mp_obj_t common_hal_wifi_radio_get_hostname(wifi_radio_obj_t *self) {
    const char *hostname = NULL;
    esp_netif_get_hostname(self->netif, &hostname);
    if (hostname == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(hostname, strlen(hostname));
}

void common_hal_wifi_radio_set_hostname(wifi_radio_obj_t *self, const char *hostname) {
    esp_netif_set_hostname(self->netif, hostname);
}

void common_hal_wifi_radio_start_station(wifi_radio_obj_t *self) {
    set_mode_station(self, true);
}

void common_hal_wifi_radio_stop_station(wifi_radio_obj_t *self) {
    set_mode_station(self, false);
}

void common_hal_wifi_radio_start_ap(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, uint8_t authmode) {
    set_mode_ap(self, true);

    switch (authmode) {
        case (1 << AUTHMODE_OPEN):
            authmode = WIFI_AUTH_OPEN;
            break;
        case ((1 << AUTHMODE_WPA) | (1 << AUTHMODE_PSK)):
            authmode = WIFI_AUTH_WPA_PSK;
            break;
        case ((1 << AUTHMODE_WPA2) | (1 << AUTHMODE_PSK)):
            authmode = WIFI_AUTH_WPA2_PSK;
            break;
        case ((1 << AUTHMODE_WPA) | (1 << AUTHMODE_WPA2) | (1 << AUTHMODE_PSK)):
            authmode = WIFI_AUTH_WPA_WPA2_PSK;
            break;
        default:
            mp_raise_ValueError(translate("Invalid AuthMode"));
            break;
    }

    wifi_config_t *config = &self->ap_config;
    memcpy(&config->ap.ssid, ssid, ssid_len);
    config->ap.ssid[ssid_len] = 0;
    memcpy(&config->ap.password, password, password_len);
    config->ap.password[password_len] = 0;
    config->ap.channel = channel;
    config->ap.authmode = authmode;
    config->ap.max_connection = 4; // kwarg?
    esp_wifi_set_config(WIFI_IF_AP, config);
}

void common_hal_wifi_radio_stop_ap(wifi_radio_obj_t *self) {
    set_mode_ap(self, false);
}

wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t *bssid, size_t bssid_len) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(translate("wifi is not enabled"));
    }

    EventBits_t bits;
    // can't block since both bits are false after wifi_init
    // both bits are true after an existing connection stops
    bits = xEventGroupWaitBits(self->event_group_handle,
        WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT,
        pdTRUE,
        pdTRUE,
        0);
    if (((bits & WIFI_CONNECTED_BIT) != 0) &&
        !((bits & WIFI_DISCONNECTED_BIT) != 0)) {
        return WIFI_RADIO_ERROR_NONE;
    }
    // explicitly clear bits since xEventGroupWaitBits may have timed out
    xEventGroupClearBits(self->event_group_handle, WIFI_CONNECTED_BIT);
    xEventGroupClearBits(self->event_group_handle, WIFI_DISCONNECTED_BIT);
    set_mode_station(self, true);

    wifi_config_t *config = &self->sta_config;
    memcpy(&config->sta.ssid, ssid, ssid_len);
    config->sta.ssid[ssid_len] = 0;
    memcpy(&config->sta.password, password, password_len);
    config->sta.password[password_len] = 0;
    config->sta.channel = channel;
    // From esp_wifi_types.h:
    //   Generally, station_config.bssid_set needs to be 0; and it needs
    //   to be 1 only when users need to check the MAC address of the AP
    if (bssid_len > 0) {
        memcpy(&config->sta.bssid, bssid, bssid_len);
        config->sta.bssid[bssid_len] = 0;
        config->sta.bssid_set = true;
    } else {
        config->sta.bssid_set = false;
    }
    // If channel is 0 (default/unset) and BSSID is not given, do a full scan instead of fast scan
    // This will ensure that the best AP in range is chosen automatically
    if ((config->sta.bssid_set == 0) && (config->sta.channel == 0)) {
        config->sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    } else {
        config->sta.scan_method = WIFI_FAST_SCAN;
    }
    esp_wifi_set_config(ESP_IF_WIFI_STA, config);
    self->starting_retries = 5;
    self->retries_left = 5;
    esp_wifi_connect();

    do {
        RUN_BACKGROUND_TASKS;
        bits = xEventGroupWaitBits(self->event_group_handle,
            WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT,
            pdTRUE,
            pdTRUE,
            0);
    } while ((bits & (WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT)) == 0 && !mp_hal_is_interrupted());
    if ((bits & WIFI_DISCONNECTED_BIT) != 0) {
        if (self->last_disconnect_reason == WIFI_REASON_AUTH_FAIL) {
            return WIFI_RADIO_ERROR_AUTH_FAIL;
        } else if (self->last_disconnect_reason == WIFI_REASON_NO_AP_FOUND) {
            return WIFI_RADIO_ERROR_NO_AP_FOUND;
        }
        return self->last_disconnect_reason;
    }
    return WIFI_RADIO_ERROR_NONE;
}

mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }

    // Make sure the interface is in STA mode
    if (!self->sta_mode) {
        return mp_const_none;
    }

    wifi_network_obj_t *ap_info = m_new_obj(wifi_network_obj_t);
    ap_info->base.type = &wifi_network_type;
    // From esp_wifi.h, the possible return values (typos theirs):
    //    ESP_OK: succeed
    //    ESP_ERR_WIFI_CONN: The station interface don't initialized
    //    ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
    if (esp_wifi_sta_get_ap_info(&self->ap_info.record) != ESP_OK) {
        return mp_const_none;
    } else {
        if (strlen(self->ap_info.record.country.cc) == 0) {
            // Workaround to fill country related information in ap_info until ESP-IDF carries a fix
            // esp_wifi_sta_get_ap_info does not appear to fill wifi_country_t (e.g. country.cc) details
            // (IDFGH-4437) #6267
            // Note: It is possible that Wi-Fi APs don't have a CC set, then even after this workaround
            //       the element would remain empty.
            memset(&self->ap_info.record.country, 0, sizeof(wifi_country_t));
            if (esp_wifi_get_country(&self->ap_info.record.country) != ESP_OK) {
                return mp_const_none;
            }
        }
        memcpy(&ap_info->record, &self->ap_info.record, sizeof(wifi_ap_record_t));
        return MP_OBJ_FROM_PTR(ap_info);
    }
}

mp_obj_t common_hal_wifi_radio_get_ipv4_gateway(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ip_info.gw.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_gateway_ap(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->ap_netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->ap_netif, &self->ap_ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ap_ip_info.gw.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_subnet(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ip_info.netmask.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_subnet_ap(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->ap_netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->ap_netif, &self->ap_ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ap_ip_info.netmask.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ip_info.ip.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address_ap(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->ap_netif)) {
        return mp_const_none;
    }
    esp_netif_get_ip_info(self->ap_netif, &self->ap_ip_info);
    return common_hal_ipaddress_new_ipv4address(self->ap_ip_info.ip.addr);
}

mp_obj_t common_hal_wifi_radio_get_ipv4_dns(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }

    esp_netif_get_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &self->dns_info);

    // dns_info is of type esp_netif_dns_info_t, which is just ever so slightly
    // different than esp_netif_ip_info_t used for
    // common_hal_wifi_radio_get_ipv4_address (includes both ipv4 and 6),
    // so some extra jumping is required to get to the actual address
    return common_hal_ipaddress_new_ipv4address(self->dns_info.ip.u_addr.ip4.addr);
}

mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout) {
    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ipaddress_ipaddress_to_esp_idf(ip_address, &ping_config.target_addr);
    ping_config.count = 1;

    size_t timeout_ms = timeout * 1000;

    esp_ping_handle_t ping;
    esp_ping_new_session(&ping_config, NULL, &ping);
    esp_ping_start(ping);

    uint32_t received = 0;
    uint32_t total_time_ms = 0;
    while (received == 0 && total_time_ms < timeout_ms && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        esp_ping_get_profile(ping, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
        esp_ping_get_profile(ping, ESP_PING_PROF_REPLY, &received, sizeof(received));
    }
    uint32_t elapsed_time = 0xffffffff;
    if (received > 0) {
        esp_ping_get_profile(ping, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    }
    esp_ping_delete_session(ping);

    return elapsed_time;
}

void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self) {
    // Only bother to scan the actual object references.
    gc_collect_ptr(self->current_scan);
}
