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

#include <string.h>

#include "common-hal/wifi/__init__.h"
#include "lib/utils/interrupt_char.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-module/ipaddress/__init__.h"

#include "esp-idf/components/esp_wifi/include/esp_wifi.h"
#include "esp-idf/components/lwip/include/apps/ping/ping_sock.h"

static void start_station(wifi_radio_obj_t *self) {
    if (self->sta_mode) {
        return;
    }
    wifi_mode_t next_mode;
    if (self->ap_mode) {
        next_mode = WIFI_MODE_APSTA;
    } else {
        next_mode = WIFI_MODE_STA;
    }
    esp_wifi_set_mode(next_mode);

    esp_wifi_set_config(WIFI_MODE_STA, &self->sta_config);
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

#define MAC_ADDRESS_LENGTH 6

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
    uint8_t mac[MAC_ADDRESS_LENGTH];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
}

mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self) {
    if (self->current_scan != NULL) {
        mp_raise_RuntimeError(translate("Already scanning for wifi networks"));
    }
    // check enabled
    start_station(self);

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
    // Free the memory used to store the found aps.
    wifi_scannednetworks_deinit(self->current_scan);
    self->current_scan = NULL;
}

wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t* ssid, size_t ssid_len, uint8_t* password, size_t password_len, uint8_t channel, mp_float_t timeout) {
    // check enabled
    wifi_config_t* config = &self->sta_config;
    memcpy(&config->sta.ssid, ssid, ssid_len);
    config->sta.ssid[ssid_len] = 0;
    memcpy(&config->sta.password, password, password_len);
    config->sta.password[password_len] = 0;
    config->sta.channel = channel;
    esp_wifi_set_config(ESP_IF_WIFI_STA, config);
    self->starting_retries = 5;
    self->retries_left = 5;
    esp_wifi_connect();

    EventBits_t bits;
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
            return WIFI_RADIO_ERROR_AUTH;
        } else if (self->last_disconnect_reason == WIFI_REASON_NO_AP_FOUND) {
            return WIFI_RADIO_ERROR_NO_AP_FOUND;
        }
        return WIFI_RADIO_ERROR_UNKNOWN;
    }
    return WIFI_RADIO_ERROR_NONE;
}

mp_obj_t common_hal_wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(self->netif, &ip_info);
    return common_hal_ipaddress_new_ipv4address(ip_info.ip.addr);
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
