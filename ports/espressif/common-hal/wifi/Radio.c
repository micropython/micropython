// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/Network.h"

#include <string.h>

#include "bindings/espidf/__init__.h"
#include "common-hal/wifi/__init__.h"
#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/ipaddress/__init__.h"

#include "components/esp_wifi/include/esp_wifi.h"
#include "components/lwip/include/apps/ping/ping_sock.h"

#if CIRCUITPY_MDNS
#include "common-hal/mdns/Server.h"
#endif

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
        #if CIRCUITPY_MDNS
        mdns_server_deinit_singleton();
        #endif
        ESP_ERROR_CHECK(esp_wifi_stop());
        self->started = false;
        return;
    }
    if (!self->started && enabled) {
        ESP_ERROR_CHECK(esp_wifi_start());
        self->started = true;
        common_hal_wifi_radio_set_tx_power(self, CIRCUITPY_WIFI_DEFAULT_TX_POWER);
        return;
    }
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
    esp_netif_set_hostname(self->ap_netif, hostname);
}

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
    uint8_t mac[MAC_ADDRESS_LENGTH];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
}

void common_hal_wifi_radio_set_mac_address(wifi_radio_obj_t *self, const uint8_t *mac) {
    if (!self->sta_mode) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Interface must be started"));
    }
    if ((mac[0] & 0b1) == 0b1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid multicast MAC address"));
    }
    esp_wifi_set_mac(ESP_IF_WIFI_STA, mac);
}

mp_float_t common_hal_wifi_radio_get_tx_power(wifi_radio_obj_t *self) {
    int8_t tx_power;
    esp_wifi_get_max_tx_power(&tx_power);
    return tx_power / 4.0f;
}

void common_hal_wifi_radio_set_tx_power(wifi_radio_obj_t *self, const mp_float_t tx_power) {
    esp_wifi_set_max_tx_power(tx_power * 4.0f);
}

mp_obj_t common_hal_wifi_radio_get_mac_address_ap(wifi_radio_obj_t *self) {
    uint8_t mac[MAC_ADDRESS_LENGTH];
    esp_wifi_get_mac(ESP_IF_WIFI_AP, mac);
    return mp_obj_new_bytes(mac, MAC_ADDRESS_LENGTH);
}

void common_hal_wifi_radio_set_mac_address_ap(wifi_radio_obj_t *self, const uint8_t *mac) {
    if (!self->ap_mode) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Interface must be started"));
    }
    if ((mac[0] & 0b1) == 0b1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid multicast MAC address"));
    }
    esp_wifi_set_mac(ESP_IF_WIFI_AP, mac);
}

mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self, uint8_t start_channel, uint8_t stop_channel) {
    if (self->current_scan != NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Already scanning for wifi networks"));
    }
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("wifi is not enabled"));
    }
    set_mode_station(self, true);

    wifi_scannednetworks_obj_t *scan = mp_obj_malloc(wifi_scannednetworks_obj_t, &wifi_scannednetworks_type);
    self->current_scan = scan;
    scan->current_channel_index = 0;
    scan->start_channel = start_channel;
    scan->end_channel = stop_channel;
    scan->radio_event_group = self->event_group_handle;
    scan->done = false;
    scan->channel_scan_in_progress = false;
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

void common_hal_wifi_radio_start_station(wifi_radio_obj_t *self) {
    set_mode_station(self, true);
}

void common_hal_wifi_radio_stop_station(wifi_radio_obj_t *self) {
    set_mode_station(self, false);
}

void common_hal_wifi_radio_start_ap(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, uint32_t authmode, uint8_t max_connections) {
    set_mode_ap(self, true);

    uint8_t esp_authmode = 0;
    switch (authmode) {
        case AUTHMODE_OPEN:
            esp_authmode = WIFI_AUTH_OPEN;
            break;
        case AUTHMODE_WPA | AUTHMODE_PSK:
            esp_authmode = WIFI_AUTH_WPA_PSK;
            break;
        case AUTHMODE_WPA2 | AUTHMODE_PSK:
            esp_authmode = WIFI_AUTH_WPA2_PSK;
            break;
        case AUTHMODE_WPA | AUTHMODE_WPA2 | AUTHMODE_PSK:
            esp_authmode = WIFI_AUTH_WPA_WPA2_PSK;
            break;
        default:
            mp_arg_error_invalid(MP_QSTR_authmode);
            break;
    }

    wifi_config_t *config = &self->ap_config;
    memcpy(&config->ap.ssid, ssid, ssid_len);
    config->ap.ssid[ssid_len] = 0;
    memcpy(&config->ap.password, password, password_len);
    config->ap.password[password_len] = 0;
    config->ap.channel = channel;
    config->ap.authmode = esp_authmode;

    mp_arg_validate_int_range(max_connections, 0, 10, MP_QSTR_max_connections);

    config->ap.max_connection = max_connections;

    esp_wifi_set_config(WIFI_IF_AP, config);
}

bool common_hal_wifi_radio_get_ap_active(wifi_radio_obj_t *self) {
    return self->ap_mode && esp_netif_is_netif_up(self->ap_netif);
}

void common_hal_wifi_radio_stop_ap(wifi_radio_obj_t *self) {
    set_mode_ap(self, false);
}

mp_obj_t common_hal_wifi_radio_get_stations_ap(wifi_radio_obj_t *self) {
    wifi_sta_list_t esp_sta_list;
    esp_err_t result;

    result = esp_wifi_ap_get_sta_list(&esp_sta_list);
    if (result != ESP_OK) {
        return mp_const_none;
    }

    esp_netif_pair_mac_ip_t mac_ip_pair[esp_sta_list.num];
    for (int i = 0; i < esp_sta_list.num; i++) {
        memcpy(mac_ip_pair[i].mac, esp_sta_list.sta[i].mac, MAC_ADDRESS_LENGTH);
        mac_ip_pair[i].ip.addr = 0;
    }

    result = esp_netif_dhcps_get_clients_by_mac(self->ap_netif, esp_sta_list.num, mac_ip_pair);
    if (result != ESP_OK) {
        return mp_const_none;
    }

    mp_obj_t mp_sta_list = mp_obj_new_list(0, NULL);
    for (int i = 0; i < esp_sta_list.num; i++) {
        mp_obj_t elems[3] = {
            mp_obj_new_bytes(esp_sta_list.sta[i].mac, MAC_ADDRESS_LENGTH),
            MP_OBJ_NEW_SMALL_INT(esp_sta_list.sta[i].rssi),
            mp_const_none
        };

        if (mac_ip_pair[i].ip.addr) {
            elems[2] = common_hal_ipaddress_new_ipv4address(mac_ip_pair[i].ip.addr);
        }

        mp_obj_list_append(mp_sta_list, namedtuple_make_new((const mp_obj_type_t *)&wifi_radio_station_type, 3, 0, elems));
    }

    return mp_sta_list;
}

wifi_radio_error_t common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t *ssid, size_t ssid_len, uint8_t *password, size_t password_len, uint8_t channel, mp_float_t timeout, uint8_t *bssid, size_t bssid_len) {
    if (!common_hal_wifi_radio_get_enabled(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("wifi is not enabled"));
    }
    wifi_config_t *config = &self->sta_config;

    size_t timeout_ms = timeout * 1000;
    uint32_t start_time = common_hal_time_monotonic_ms();
    uint32_t end_time = start_time + timeout_ms;

    EventBits_t bits;
    // can't block since both bits are false after wifi_init
    // both bits are true after an existing connection stops
    bits = xEventGroupWaitBits(self->event_group_handle,
        WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT,
        pdTRUE,
        pdTRUE,
        0);
    bool connected = ((bits & WIFI_CONNECTED_BIT) != 0) &&
        !((bits & WIFI_DISCONNECTED_BIT) != 0);
    if (connected) {
        // SSIDs are up to 32 bytes. Assume it is null terminated if it is less.
        if (memcmp(ssid, config->sta.ssid, ssid_len) == 0 &&
            (ssid_len == 32 || strlen((const char *)config->sta.ssid) == ssid_len)) {
            // Already connected to the desired network.
            return WIFI_RADIO_ERROR_NONE;
        } else {
            xEventGroupClearBits(self->event_group_handle, WIFI_DISCONNECTED_BIT);
            // Trying to switch networks so disconnect first.
            esp_wifi_disconnect();
            do {
                RUN_BACKGROUND_TASKS;
                bits = xEventGroupWaitBits(self->event_group_handle,
                    WIFI_DISCONNECTED_BIT,
                    pdTRUE,
                    pdTRUE,
                    0);
            } while ((bits & WIFI_DISCONNECTED_BIT) == 0 && !mp_hal_is_interrupted());
        }
    }
    // explicitly clear bits since xEventGroupWaitBits may have timed out
    xEventGroupClearBits(self->event_group_handle, WIFI_CONNECTED_BIT);
    xEventGroupClearBits(self->event_group_handle, WIFI_DISCONNECTED_BIT);
    set_mode_station(self, true);

    memcpy(&config->sta.ssid, ssid, ssid_len);
    if (ssid_len < 32) {
        config->sta.ssid[ssid_len] = 0;
    }
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
        // Don't retry anymore if we're over our time budget.
        if (self->retries_left > 0 && common_hal_time_monotonic_ms() > end_time) {
            self->retries_left = 0;
        }
    } while ((bits & (WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT)) == 0 && !mp_hal_is_interrupted());

    if ((bits & WIFI_DISCONNECTED_BIT) != 0) {
        if (
            (self->last_disconnect_reason == WIFI_REASON_AUTH_FAIL) ||
            (self->last_disconnect_reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT) ||
            (self->last_disconnect_reason == WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY) ||
            (self->last_disconnect_reason == WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD)
            ) {
            return WIFI_RADIO_ERROR_AUTH_FAIL;
        } else if (self->last_disconnect_reason == WIFI_REASON_NO_AP_FOUND) {
            return WIFI_RADIO_ERROR_NO_AP_FOUND;
        }
        return self->last_disconnect_reason;
    } else {
        // We're connected, allow us to retry if we get disconnected.
        self->retries_left = self->starting_retries;
    }
    return WIFI_RADIO_ERROR_NONE;
}

bool common_hal_wifi_radio_get_connected(wifi_radio_obj_t *self) {
    return self->sta_mode && esp_netif_is_netif_up(self->netif);
}

mp_obj_t common_hal_wifi_radio_get_ap_info(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return mp_const_none;
    }

    // Make sure the interface is in STA mode
    if (!self->sta_mode) {
        return mp_const_none;
    }

    wifi_network_obj_t *ap_info = mp_obj_malloc(wifi_network_obj_t, &wifi_network_type);
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

uint32_t wifi_radio_get_ipv4_address(wifi_radio_obj_t *self) {
    if (!esp_netif_is_netif_up(self->netif)) {
        return 0;
    }
    esp_netif_get_ip_info(self->netif, &self->ip_info);
    return self->ip_info.ip.addr;
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

void common_hal_wifi_radio_set_ipv4_dns(wifi_radio_obj_t *self, mp_obj_t ipv4_dns_addr) {
    esp_netif_dns_info_t dns_addr;
    ipaddress_ipaddress_to_esp_idf_ip4(ipv4_dns_addr, &dns_addr.ip.u_addr.ip4);
    esp_netif_set_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &dns_addr);
}

void common_hal_wifi_radio_start_dhcp_client(wifi_radio_obj_t *self) {
    esp_netif_dhcpc_start(self->netif);
}

void common_hal_wifi_radio_stop_dhcp_client(wifi_radio_obj_t *self) {
    esp_netif_dhcpc_stop(self->netif);
}

void common_hal_wifi_radio_start_dhcp_server(wifi_radio_obj_t *self) {
    esp_netif_dhcps_start(self->ap_netif);
}

void common_hal_wifi_radio_stop_dhcp_server(wifi_radio_obj_t *self) {
    esp_netif_dhcps_stop(self->ap_netif);
}

void common_hal_wifi_radio_set_ipv4_address(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns) {
    common_hal_wifi_radio_stop_dhcp_client(self); // Must stop station DHCP to set a manual address

    esp_netif_ip_info_t ip_info;
    ipaddress_ipaddress_to_esp_idf_ip4(ipv4, &ip_info.ip);
    ipaddress_ipaddress_to_esp_idf_ip4(netmask, &ip_info.netmask);
    ipaddress_ipaddress_to_esp_idf_ip4(gateway, &ip_info.gw);

    esp_netif_set_ip_info(self->netif, &ip_info);

    if (ipv4_dns != MP_OBJ_NULL) {
        common_hal_wifi_radio_set_ipv4_dns(self, ipv4_dns);
    }
}

void common_hal_wifi_radio_set_ipv4_address_ap(wifi_radio_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway) {
    common_hal_wifi_radio_stop_dhcp_server(self); // Must stop access point DHCP to set a manual address

    esp_netif_ip_info_t ip_info;
    ipaddress_ipaddress_to_esp_idf_ip4(ipv4, &ip_info.ip);
    ipaddress_ipaddress_to_esp_idf_ip4(netmask, &ip_info.netmask);
    ipaddress_ipaddress_to_esp_idf_ip4(gateway, &ip_info.gw);

    esp_netif_set_ip_info(self->ap_netif, &ip_info);

    common_hal_wifi_radio_start_dhcp_server(self); // restart access point DHCP
}

static void ping_success_cb(esp_ping_handle_t hdl, void *args) {
    wifi_radio_obj_t *self = (wifi_radio_obj_t *)args;
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &self->ping_elapsed_time, sizeof(self->ping_elapsed_time));
}

mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address, mp_float_t timeout) {
    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ipaddress_ipaddress_to_esp_idf(ip_address, &ping_config.target_addr);
    ping_config.count = 1;

    // We must fetch ping information using the callback mechanism, because the session storage is freed when
    // the ping session is done, even before esp_ping_delete_session().
    esp_ping_callbacks_t ping_callbacks = {
        .on_ping_success = ping_success_cb,
        .cb_args = (void *)self,
    };

    size_t timeout_ms = timeout * 1000;

    // ESP-IDF creates a task to do the ping session. It shuts down when done, but only after a one second delay.
    // Calling common_hal_wifi_radio_ping() too fast will cause resource exhaustion.
    esp_ping_handle_t ping;
    if (esp_ping_new_session(&ping_config, &ping_callbacks, &ping) != ESP_OK) {
        // Wait for old task to go away and then try again.
        // Empirical testing shows we have to wait at least two seconds, despite the task
        // having a one-second timeout.
        common_hal_time_delay_ms(2000);
        // Return if interrupted now, to show the interruption as KeyboardInterrupt instead of the
        // IDF error.
        if (mp_hal_is_interrupted()) {
            return (uint32_t)(-1);
        }
        CHECK_ESP_RESULT(esp_ping_new_session(&ping_config, &ping_callbacks, &ping));
    }

    esp_ping_start(ping);

    // Use all ones as a flag that the elapsed time was not set (ping failed or timed out).
    self->ping_elapsed_time = (uint32_t)(-1);

    uint32_t start_time = common_hal_time_monotonic_ms();
    while ((self->ping_elapsed_time == (uint32_t)(-1)) &&
           (common_hal_time_monotonic_ms() - start_time < timeout_ms) &&
           !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    esp_ping_stop(ping);
    esp_ping_delete_session(ping);

    return (mp_int_t)self->ping_elapsed_time;
}

void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self) {
    // Only bother to scan the actual object references.
    gc_collect_ptr(self->current_scan);
}
